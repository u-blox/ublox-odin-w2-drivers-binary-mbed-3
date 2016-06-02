
/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2016 u-blox AB, Sweden.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This test verifies that it's possible to do the following:
 * - Initialize WiFi driver
 * - Start WiFi driver
 * - Start LWIP
 * - Start access point mode with WPA2 security and DHCP server
 * - Provide an IP address to the connected Wi-Fi client
 * - Reply to ping requests
 */

#include <stdio.h>
#include <string.h>

#include "mbed-drivers/mbed.h"
#include "minar/minar.h"
#include "sockets/UDPSocket.h"

#include "sal-stack-lwip-ublox-odin-w2/lwipv4_init.h"
#include "ublox-odin-w2-drivers/cb_wlan.h"
#include "ublox-odin-w2-drivers/cb_wlan_types.h"
#include "ublox-odin-w2-drivers/cb_main.h"
#include "ublox-odin-w2-lwip-adapt/cb_ip.h"

using namespace mbed::util;

/*===========================================================================
* DEFINES
*=========================================================================*/

#define APP_HOSTNAME "my_hostname"
#define APP_IP_ADDR_MAX_LEN 15
#define APP_IP6_ADDR_MAX_LEN 39

#define APP_MASK_LOWESTBIT(BITMASK)           ((BITMASK) & (-(BITMASK)))
#define APP_MASK_SHIFTDOWN(MASK, SHIFTMASK)   ((cbWLAN_RateMask)((SHIFTMASK != 0 ) ? (MASK / APP_MASK_LOWESTBIT(SHIFTMASK)) : MASK))
#define APP_MASK_SHIFTUP(MASK, SHIFTMASK)     ((cbWLAN_RateMask)((SHIFTMASK != 0 ) ? (MASK * APP_MASK_LOWESTBIT(SHIFTMASK)) : MASK))

#ifndef YOTTA_CFG_TEST_WIFI_AP_SSID
#define YOTTA_CFG_TEST_WIFI_AP_SSID "my_ssid"
#endif
#ifndef YOTTA_CFG_TEST_WIFI_AP_PASSPHRASE
#define YOTTA_CFG_TEST_WIFI_AP_PASSPHRASE "my_passphrase"
#endif
#ifndef YOTTA_CFG_TEST_WIFI_AP_CHANNEL
#define YOTTA_CFG_TEST_WIFI_AP_CHANNEL 6
#endif
#ifndef YOTTA_CFG_TEST_IP_STATIC_ADDR
#define YOTTA_CFG_TEST_IP_STATIC_ADDR "192.168.0.1"
#endif
#ifndef YOTTA_CFG_TEST_IP_STATIC_NETMASK
#define YOTTA_CFG_TEST_IP_STATIC_NETMASK "255.255.255.0"
#endif
#ifndef YOTTA_CFG_TEST_IP_STATIC_GATEWAY
#define YOTTA_CFG_TEST_IP_STATIC_GATEWAY "192.168.0.1"
#endif
#ifndef YOTTA_CFG_TEST_IP_STATIC_DNS_0
#define YOTTA_CFG_TEST_IP_STATIC_DNS_0 "0.0.0.0"
#endif
#ifndef YOTTA_CFG_TEST_IP_STATIC_DNS_1
#define YOTTA_CFG_TEST_IP_STATIC_DNS_1 "0.0.0.0"
#endif

/*===========================================================================
* TYPES
*=========================================================================*/

typedef struct
{
    bool interfaceUp;
    cbIP_interfaceSettings ifSettings;
    cbIP_IPv4Settings currentIPv4Settings;
    cbIP_IPv6Settings currentIPv6Settings;
} APP_admin;

/*===========================================================================
* DECLARATIONS
*=========================================================================*/

static void handleStatusIndication(void *callbackContext, cbWLAN_StatusIndicationInfo status, void *data);
static void scheduledApStartWpa2();
static void scheduledStartInterfaces();
static void handleIpStatusIndication(
    cbIP_Status status,
    void* callbackArg,
    cbIP_IPv4Settings const * const ipV4settings,
    cbIP_IPv6Settings const * const ipV6settings);

/*===========================================================================
* DEFINITIONS
*=========================================================================*/

static char* _ssid = (char*)YOTTA_CFG_TEST_WIFI_AP_SSID;
static char* _passphrase = (char*)YOTTA_CFG_TEST_WIFI_AP_PASSPHRASE;
static cbWLAN_Channel _channel = YOTTA_CFG_TEST_WIFI_AP_CHANNEL;
static char* _addr = (char*)YOTTA_CFG_TEST_IP_STATIC_ADDR;
static char* _netmask = (char*)YOTTA_CFG_TEST_IP_STATIC_NETMASK;
static char* _gateway = (char*)YOTTA_CFG_TEST_IP_STATIC_GATEWAY;
static char* _dns0 = (char*)YOTTA_CFG_TEST_IP_STATIC_DNS_0;
static char* _dns1 = (char*)YOTTA_CFG_TEST_IP_STATIC_DNS_1;
static APP_admin _admin;

/*===========================================================================
* FUNCTIONS/CLASSES
*=========================================================================*/

static void scheduledApStartWpa2()
{
    cbWLAN_CommonApParameters apParameters;
    char tempPassphrase[cbWLAN_MAX_PASSPHRASE_LENGTH];
    cbWLAN_WPAPSKApParameters wpaApParameters;

    memset(&apParameters, 0, sizeof(apParameters));
    apParameters.ssid.ssidLength = strlen(_ssid);
    memcpy(apParameters.ssid.ssid, _ssid, apParameters.ssid.ssidLength);
    apParameters.channel = _channel;
    apParameters.basicRates = cbRATE_MASK_01 | APP_MASK_SHIFTUP(cbRATE_MASK_01, cbRATE_MASK_G);

    memset(&wpaApParameters, 0, sizeof(wpaApParameters));
    wpaApParameters.rsnCiphers = cbWLAN_CIPHER_SUITE_AES_CCMP;
    memset(tempPassphrase, 0, cbWLAN_MAX_PASSPHRASE_LENGTH);
    memcpy(tempPassphrase, _passphrase, strlen(_passphrase));
    cbWLAN_Util_PSKFromPWD(tempPassphrase, apParameters.ssid, wpaApParameters.psk.key);

    cbWLAN_apStartWPAPSK(&apParameters, &wpaApParameters);
}

static void scheduledStartInterfaces()
{
    cbIP_IPv6Settings ipv6settings;
    cbIP_IPv4Settings ipv4settings;

    cbIP_UTIL_aton(_addr, &ipv4settings.address);
    cbIP_UTIL_aton(_netmask, &ipv4settings.netmask);
    cbIP_UTIL_aton(_gateway, &ipv4settings.gateway);
    cbIP_UTIL_aton(_dns0, &ipv4settings.dns0);
    cbIP_UTIL_aton(_dns1, &ipv4settings.dns1);

    _admin.ifSettings.dhcpServerState = cbIP_DHCP_SERVER_ENABLE;
    cbIP_initWlanInterfaceStatic((char*)APP_HOSTNAME, &ipv4settings, &ipv6settings, &_admin.ifSettings, handleIpStatusIndication, NULL);
}

static void handleStatusIndication(void *callbackContext, cbWLAN_StatusIndicationInfo status, void *data)
{
    (void)callbackContext;

    switch (status) {
        case cbWLAN_STATUS_STOPPED:
            printf("cbWLAN_STATUS_STOPPED\n");
            break;
        case cbWLAN_STATUS_STARTED:
            {
                cbWLAN_StatusStartedInfo* startedInfo = (cbWLAN_StatusStartedInfo*)data;
                memcpy(_admin.ifSettings.macAddress,startedInfo->macAddress,sizeof(_admin.ifSettings.macAddress));
                _admin.ifSettings.MTU = 1517;

                printf("cbWLAN_STATUS_STARTED - MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                    startedInfo->macAddress[0], 
                    startedInfo->macAddress[1], 
                    startedInfo->macAddress[2], 
                    startedInfo->macAddress[3], 
                    startedInfo->macAddress[4], 
                    startedInfo->macAddress[5]);

                minar::Scheduler::postCallback(scheduledApStartWpa2);
            }
            break;
        case cbWLAN_STATUS_ERROR:
            printf("cbWLAN_STATUS_ERROR\n");
            break;
        case cbWLAN_STATUS_DISCONNECTED:
            printf("cbWLAN_STATUS_DISCONNECTED\n");
            break;
        case cbWLAN_STATUS_CONNECTION_FAILURE:
            {
                char *pInfoTxt = NULL;
                cbWLAN_StatusDisconnectedInfo info;
                std::memcpy(&info, &data, sizeof info);
                switch (info)
                {
                case cbWLAN_STATUS_DISCONNECTED_UNKNOWN:
                    pInfoTxt = (char*)"UNKNOWN";
                    break;
                case cbWLAN_STATUS_DISCONNECTED_NO_BSSID_FOUND:
                    pInfoTxt = (char*)"NO_BSSID_FOUND";
                    break;
                case cbWLAN_STATUS_DISCONNECTED_AUTH_TIMEOUT:
                    pInfoTxt = (char*)"AUTH_TIMEOUT";
                    break;
                case cbWLAN_STATUS_DISCONNECTED_MIC_FAILURE:
                    pInfoTxt = (char*)"MIC FAILURE";
                    break;
                default:
                    pInfoTxt = (char*)"ERROR";
                    break;
                }
                printf("cbWLAN_STATUS_CONNECTION_FAILURE - %s\n", pInfoTxt);
            }
            break;
        case cbWLAN_STATUS_CONNECTING:
            printf("cbWLAN_STATUS_CONNECTING\n");
            break;
        case cbWLAN_STATUS_CONNECTED:
            {
                cbWLAN_StatusConnectedInfo* connectedInfo = (cbWLAN_StatusConnectedInfo*)data;
                printf("cbWLAN_STATUS_CONNECTED - %02x:%02x:%02x:%02x:%02x:%02x ch=%d\n",
                    connectedInfo->bssid[0], connectedInfo->bssid[1], connectedInfo->bssid[2],
                    connectedInfo->bssid[3], connectedInfo->bssid[4], connectedInfo->bssid[5], 
                    connectedInfo->channel);
            }
            break;
        case cbWLAN_STATUS_AP_UP:
            {
                cbWLAN_ApInformation* apInfo = (cbWLAN_ApInformation*)data;
                cb_char _ssid[cbWLAN_SSID_MAX_LENGTH + 1];
                memset(_ssid, '\0', sizeof(_ssid));
                memcpy(_ssid, apInfo->ssid.ssid, apInfo->ssid.ssidLength);
                printf("cbWLAN_STATUS_AP_UP - \"%s\",0x%02x:%02x:%02x:%02x:%02x:%02x ch=%d\n",
                    _ssid,
                    apInfo->bssid[0], apInfo->bssid[1],
                    apInfo->bssid[2], apInfo->bssid[3],
                    apInfo->bssid[4], apInfo->bssid[5],
                    apInfo->channel);
            }
            break;
        case cbWLAN_STATUS_AP_DOWN:
            printf("cbWLAN_STATUS_AP_DOWN\n");
            break;
        case cbWLAN_STATUS_AP_STA_ADDED:
            {
                cbWLAN_ApStaInformation* staInfo = (cbWLAN_ApStaInformation*)data;
                printf("cbWLAN_STATUS_AP_STA_ADDED - 0x%02x:%02x:%02x:%02x:%02x:%02x\n",
                    staInfo->MAC[0], staInfo->MAC[1],
                    staInfo->MAC[2], staInfo->MAC[3],
                    staInfo->MAC[4], staInfo->MAC[5]);
            }
            break;
        case cbWLAN_STATUS_AP_STA_REMOVED:
            {
                cbWLAN_ApStaInformation* staInfo = (cbWLAN_ApStaInformation*)data;
                printf("cbWLAN_STATUS_AP_STA_REMOVED - 0x%02x:%02x:%02x:%02x:%02x:%02x\n",
                    staInfo->MAC[0], staInfo->MAC[1],
                    staInfo->MAC[2], staInfo->MAC[3],
                    staInfo->MAC[4], staInfo->MAC[5]);
            }
            break;
        default:
            CORE_UTIL_ASSERT(0);
            break;
    }
}

static void handleIpStatusIndication(
    cbIP_Status status,
    void* callbackArg,
    cbIP_IPv4Settings const * const ipV4settings,
    cbIP_IPv6Settings const * const ipV6settings)
{
    (void)callbackArg;

    switch(status)
    {
    case cbIP_NETWORK_UP:
        {
            char addr[APP_IP_ADDR_MAX_LEN + 1];
            char addr6[APP_IP6_ADDR_MAX_LEN + 1];
            addr[APP_IP_ADDR_MAX_LEN] = 0;
            _admin.currentIPv4Settings = *ipV4settings;
            _admin.currentIPv6Settings = *ipV6settings;

            cbIP_UTIL_ntoa(&_admin.currentIPv4Settings.address, addr, APP_IP_ADDR_MAX_LEN);
            cbIP_UTIL_ip6addr_ntoa(&_admin.currentIPv6Settings.linklocal, addr6, APP_IP6_ADDR_MAX_LEN);

            cbIP_setDefaultNetif(ipV4settings->address);

            printf("Network up\n");
            printf("IPv4 address: %s\n", addr);
            printf("IPv6 address: %s\n", addr6);
            _admin.interfaceUp = TRUE;
        }
        break;
    case cbIP_NETWORK_DOWN:
        {
            memset(&_admin.currentIPv4Settings, 0, sizeof(cbIP_IPv4Settings));
            _admin.interfaceUp = FALSE;
            printf("Network down\n");
        }
        break;
    case cbIP_NETWORK_ACTIVITY:
        break;
    }
}

void app_start(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    cb_int32 wlanTargetId = cbMAIN_initWlan();

    socket_error_t err = lwipv4_socket_init();
    MBED_ASSERT(err == SOCKET_ERROR_NONE);

    cbWLAN_StartParameters startParams;
    memset(&startParams, 0, sizeof(startParams));
    startParams.disable80211d = FALSE; // The driver must always support this feature
    startParams.deviceType = cbWM_MODULE_ODIN_W26X; // ODIN-W2 is the only applicable for ARM mbed
    startParams.deviceSpecific.ODIN_W26X.txPowerSettings.lowTxPowerLevel = cbWLAN_TX_POWER_AUTO;
    startParams.deviceSpecific.ODIN_W26X.txPowerSettings.medTxPowerLevel = cbWLAN_TX_POWER_AUTO;
    startParams.deviceSpecific.ODIN_W26X.txPowerSettings.maxTxPowerLevel = cbWLAN_TX_POWER_AUTO;

    cbIP_init();

    minar::Scheduler::postCallback(scheduledStartInterfaces);

    cbWLAN_registerStatusCallback(handleStatusIndication, NULL);
    cbMAIN_startWlan(wlanTargetId,&startParams);
}
