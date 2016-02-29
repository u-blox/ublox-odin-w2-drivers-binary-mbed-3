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
 * - Connect WiFi client with WPA2 security to an access point
 * - Start LWIP
 * - Get IP address with DHCP
 * - Resolve time server host name
 * - Get time from time server
 *
 *   To run the test the _ssid and _passphrase must be replaced by the 
 *   credentials of your access point.
 *
 *   The code below mixes C and C++. The reason for this is that the 
 *   WiFi interface is purely C while the UDP socket is C++.
 *  
 *   TODO: Remove interface and disconnect after receiving the time
 *   TODO: Add minimal state machine
 */

#include <stdio.h>
#include <string.h>

#include "mbed-drivers/mbed.h"
#include "mbed-drivers/test_env.h"
#include "sockets/TCPStream.h"
#include "sal/test/ctest_env.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "sockets/UDPSocket.h"

#include "sal-stack-lwip-ublox-odin-w2/lwipv4_init.h"
#include "ublox-odin-w2-drivers/cb_wlan.h"
#include "ublox-odin-w2-drivers/cb_wlan_types.h"
#include "ublox-odin-w2-drivers/cb_main.h"
#include "ublox-odin-w2-lwip-adapt/cb_ip.h"

using namespace mbed::util;
using namespace mbed::Sockets::v0;

/*===========================================================================
* DEFINES
*=========================================================================*/

#define APP_HOSTNAME "my_hostname"
#define APP_MAX_HOSTNAME_LENGTH 64
#define APP_IP_ADDR_MAX_LEN 15
#define APP_IP6_ADDR_MAX_LEN 39
#define APP_TIME_SRV_PORT 37
#define UDP_TIME_CLIENT_BUFFER_SIZE 64

#ifndef YOTTA_CFG_TEST_TIME_SRV_NAME
#define YOTTA_CFG_TEST_TIME_SRV_NAME "nist-time-server.eoni.com"
#endif
#ifndef YOTTA_CFG_TEST_WIFI_SSID
#define YOTTA_CFG_TEST_WIFI_SSID "my_ssid"
#endif
#ifndef YOTTA_CFG_TEST_WIFI_PASSPHRASE
#define YOTTA_CFG_TEST_WIFI_PASSPHRASE "my_passphrase"
#endif

/*===========================================================================
* TYPES
*=========================================================================*/

typedef struct
{
    cbIP_IPv4Settings ipv4Settings;
    bool dhcpEnabled;
    char hostName[APP_MAX_HOSTNAME_LENGTH];
    cbIP_IPv6Settings ipv6Settings;
} APP_networkConfig;

typedef struct
{
    APP_networkConfig network;
    bool interfaceUp;
    cbIP_interfaceSettings ifSettings;
    cbIP_IPv4Settings currentIPv4Settings;
    cbIP_IPv6Settings currentIPv6Settings;
    cbIP_IPv4Address resolvedAddr;
} APP_admin;

class UDPTimeClient;
typedef mbed::util::FunctionPointer2<void, bool, UDPTimeClient*> fpterminate_t;

/*===========================================================================
* DECLARATIONS
*=========================================================================*/

static void terminate(bool status, UDPTimeClient* client);
static void handleStatusIndication(void *callbackContext, cbWLAN_StatusIndicationInfo status, void *data);
static void scheduledConnectWpa2();
static void scheduledStartInterfaces();
static void handleIpStatusIndication(
    cbIP_Status status,
    void* callbackArg,
    cbIP_IPv4Settings const * const ipV4settings,
    cbIP_IPv6Settings const * const ipV6settings);

/*===========================================================================
* DEFINITIONS
*=========================================================================*/
UDPTimeClient *_timeClient;
static char* _ssid = (char*)YOTTA_CFG_TEST_WIFI_SSID;
static char* _passphrase = (char*)YOTTA_CFG_TEST_WIFI_PASSPHRASE;
static APP_admin _admin;

/*===========================================================================
* FUNCTIONS/CLASSES
*=========================================================================*/

class UDPTimeClient {
public:
    UDPTimeClient(socket_stack_t stack) :
        _usock(stack)
    {
        _usock.setOnError(UDPSocket::ErrorHandler_t(this, &UDPTimeClient::onError));
    }
    void getTime(char *host_addr, uint16_t port)
    {
        _port = port;

	    socket_error_t err = _usock.open(SOCKET_AF_INET4);
	    TEST_EQ(err, SOCKET_ERROR_NONE);

        err = _usock.resolve(host_addr,UDPSocket::DNSHandler_t(this, &UDPTimeClient::onDNS));
        if(!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            printf("MBED: Failed to resolve %s\r\n", host_addr);
            onError(&_usock, err);
        }
    }
    void onError(Socket *s, socket_error_t err) {
        (void) s;
        printf("MBED: Socket Error: %s (%d)\r\n", socket_strerror(err), err);
        minar::Scheduler::postCallback(fpterminate_t(terminate).bind(TEST_RESULT(),this));
    }
    void onDNS(Socket *s, struct socket_addr sa, const char * domain)
    {
        (void) s;
        /* Extract the Socket event to read the resolved address */
        _resolvedAddr.setAddr(&sa);
        _resolvedAddr.fmtIPv6(out_buffer, sizeof(out_buffer));
        printf("MBED: Resolved %s to %s\r\n", domain, out_buffer);

        /* Register the read handler */
        _usock.setOnReadable(UDPSocket::ReadableHandler_t(this, &UDPTimeClient::onRx));
        /* Send the query packet to the remote host */
        _usock.send_to("test", 4, &_resolvedAddr, _port);
    }
    void onRx(Socket *s)
    {
        (void)s;
        unsigned int n = sizeof(out_buffer);
        socket_error_t err = _usock.recv(out_buffer, &n);
        TEST_EQ(err, SOCKET_ERROR_NONE);

        uint32_t timeInSeconds = (out_buffer[0] << 24) + (out_buffer[1] << 16) + (out_buffer[2] << 8) + (out_buffer[3] << 0);
        printf("UDP time in seconds since 1900: %lu\n", timeInSeconds);
        minar::Scheduler::postCallback(fpterminate_t(terminate).bind(TEST_RESULT(),this));
    }
    bool isError() {
        return !TEST_RESULT();
    }

protected:
    UDPSocket _usock;
    SocketAddr _resolvedAddr;
    uint16_t _port;
    char out_buffer[UDP_TIME_CLIENT_BUFFER_SIZE];
};

static void terminate(bool status, UDPTimeClient* client)
{
    delete client;
    MBED_HOSTTEST_RESULT(status);
}

static void scheduledConnectWpa2()
{
    char tempPassphrase[cbWLAN_MAX_PASSPHRASE_LENGTH];
    cbWLAN_CommonConnectParameters connectParams;
    cbWLAN_WPAPSKConnectParameters wpaConnectParams;

    memset(&connectParams, 0, sizeof(connectParams));
    memset(&wpaConnectParams, 0, sizeof(wpaConnectParams));
    memset(tempPassphrase, 0, cbWLAN_MAX_PASSPHRASE_LENGTH);
    memcpy(tempPassphrase, _passphrase, strlen(_passphrase));

    connectParams.ssid.ssidLength = strlen(_ssid);
    memcpy(connectParams.ssid.ssid, _ssid, connectParams.ssid.ssidLength);
    cbWLAN_Util_PSKFromPWD(tempPassphrase, connectParams.ssid, wpaConnectParams.psk);
    cbWLAN_connectWPAPSK(&connectParams, &wpaConnectParams);
}

static void scheduledStartInterfaces()
{
    cbIP_IPv6Settings ipv6settings;

    cbIP_initWlanInterfaceDHCP((char*)APP_HOSTNAME, &ipv6settings, &_admin.ifSettings, handleIpStatusIndication, NULL);
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

                minar::Scheduler::postCallback(scheduledConnectWpa2);
                break;
            }
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
                if(_admin.interfaceUp == TRUE) {
                    cbIP_removeWlanInterface();
                }
                minar::Scheduler::postCallback(scheduledStartInterfaces);
                break;
            }
        case cbWLAN_STATUS_AP_UP:
            printf("cbWLAN_STATUS_AP_UP\n");
            break;
        case cbWLAN_STATUS_AP_DOWN:
            printf("cbWLAN_STATUS_AP_DOWN\n");
            break;
        case cbWLAN_STATUS_AP_STA_ADDED:
            printf("cbWLAN_STATUS_AP_STA_ADDED\n");
            break;
        case cbWLAN_STATUS_AP_STA_REMOVED:
            printf("cbWLAN_STATUS_AP_STA_REMOVED\n");
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

            mbed::util::FunctionPointer2<void, char *, uint16_t> fp(_timeClient, &UDPTimeClient::getTime);
            minar::Scheduler::postCallback(fp.bind((char*)YOTTA_CFG_TEST_TIME_SRV_NAME, APP_TIME_SRV_PORT));
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
 /* Ignore for now */
}


void app_start(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    cbMAIN_initWlan();

    MBED_HOSTTEST_TIMEOUT(30);
    MBED_HOSTTEST_SELECT(tcpecho_client_auto);
    MBED_HOSTTEST_DESCRIPTION(TCP echo client);
    MBED_HOSTTEST_START("NET_4");
    socket_error_t err = lwipv4_socket_init();
    TEST_EQ(err, SOCKET_ERROR_NONE);

    cbWLAN_StartParameters startParams;
    memset(&startParams, 0, sizeof(startParams));
    startParams.disable80211d = FALSE; // The driver must always support this feature
    startParams.deviceType = cbWM_MODULE_ODIN_W26X; // ODIN-W2 is the only applicable for ARM mbed
    startParams.deviceSpecific.ODIN_W26X.txPowerSettings.lowTxPowerLevel = cbWLAN_TX_POWER_AUTO;
    startParams.deviceSpecific.ODIN_W26X.txPowerSettings.medTxPowerLevel = cbWLAN_TX_POWER_AUTO;
    startParams.deviceSpecific.ODIN_W26X.txPowerSettings.maxTxPowerLevel = cbWLAN_TX_POWER_AUTO;

    cbIP_init();

    cbWLAN_registerStatusCallback(handleStatusIndication, NULL);
    cbWLAN_start(&startParams);

    _timeClient = new UDPTimeClient(SOCKET_STACK_LWIP_IPV4);
}
