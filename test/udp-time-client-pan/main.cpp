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
 * - Initialize Bluetooth stack
 * - Connect PANU to NAP(Network Access Point)
 * - Start LWIP
 * - Get IP address with DHCP
 * - Resolve time server host name
 * - Get time from time server
 *
 *   To run the test the NAP Bluetooth address must be configured
 *   below in the code or in a config.json file with YOTTA_CFG_TEST_BT_REMOTE_NAP_ADDR.
 *   For example:
 *   "test": {
 *     "bt_remote_nap_addr": "0xd8, 0xbb, 0x2c, 0x69, 0x4f, 0xf2"
 *   }
 *
 *   The code below mixes C and C++. The reason for this is that the 
 *   WiFi interface is purely C while the UDP socket is C++.
 *  
 *   TODO: Remove interface and disconnect after receiving the time
 *   TODO: Add minimal state machine
 *
 *   An iPhone or Android phone with Internet sharing/tethering can be used as NAP
 */

#include "mbed-drivers/mbed.h"
#include "greentea-client/test_env.h"
#include "sockets/TCPStream.h"
#include "sal/test/ctest_env.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "sockets/UDPSocket.h"

#include "sal-stack-lwip-ublox-odin-w2/lwipv4_init.h"
#include "ublox-odin-w2-drivers/cb_main.h"
#include "ublox-odin-w2-lwip-adapt/cb_ip.h"
#include "ublox-odin-w2-drivers/cb_assert.h"
#include "ublox-odin-w2-drivers/bt_types.h"
#include "ublox-odin-w2-drivers/cb_bt_utils.h"
#include "ublox-odin-w2-drivers/cb_bt_pan.h"
#include "ublox-odin-w2-drivers/cb_bt_man.h"
#include "ublox-odin-w2-drivers/cb_bt_conn_man.h"
#include "ublox-odin-w2-drivers/cb_bt_sec_man.h"

#include <string.h>
#include <stdio.h>

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

#ifndef YOTTA_CFG_TEST_BT_REMOTE_NAP_ADDR
#warning YOTTA_CFG_TEST_BT_REMOTE_NAP_ADDR must be defined in a config.json file
#define YOTTA_CFG_TEST_BT_REMOTE_NAP_ADDR 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
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
    cbBCM_Handle connHandle;
} APP_admin;

class UDPTimeClient;
typedef mbed::util::FunctionPointer2<void, bool, UDPTimeClient*> fpterminate_t;

/*===========================================================================
* DECLARATIONS
*=========================================================================*/

static void terminate(bool status, UDPTimeClient* client);
static void scheduledConnectPan();
static void scheduledStartInterfaces();
static void handleIpStatusIndication(
    cbIP_Status status,
    void* callbackArg,
    cbIP_IPv4Settings const * const ipV4settings,
    cbIP_IPv6Settings const * const ipV6settings);

static void controllerStartupComplete(void);
static void initBt(void);

static void handleRequestPinInd(TBdAddr* pBdAddress);
static void handleUserConfirmationInd(TBdAddr* pBdAddress, cb_uint32 numericValue);
static void handleUserPasskeyInd(TBdAddr* pBdAddress);
static void handleUserPasskeyEvt(TBdAddr* pBdAddress, cb_uint32 passkey);

static void handleConnectInd(cbBCM_Handle handle, cbBCM_ConnectionInfo info);
static void handleConnectEvt(cbBCM_Handle handle, cbBCM_ConnectionInfo info);
static void handleConnectCnf(cbBCM_Handle handle, cbBCM_ConnectionInfo info, cb_int32 status);
static void handleDisconnectEvt(cbBCM_Handle handle);

/*===========================================================================
* DEFINITIONS
*=========================================================================*/
static UDPTimeClient *_timeClient;
static TBdAddr _napAddr = { { YOTTA_CFG_TEST_BT_REMOTE_NAP_ADDR }, BT_PUBLIC_ADDRESS };
static APP_admin _admin;

static const cbBSM_Callbacks _securityCallbacks =
{
    handleRequestPinInd,
    handleUserConfirmationInd,
    handleUserPasskeyInd,
    handleUserPasskeyEvt,
    NULL,
    NULL
};

static const cbBCM_ConnectionCallback _connectionCallbacks =
{
    handleConnectInd,
    handleConnectEvt,
    handleConnectCnf,
    handleDisconnectEvt
};

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
    GREENTEA_TESTSUITE_RESULT(status);
}

static void controllerStartupComplete(void)
{
    TBdAddr addr;
    cb_int32 result;

    initBt();

    result = cbBM_getLocalAddress(&addr);
    cb_ASSERT(result == cbBM_OK);
    memcpy(_admin.ifSettings.macAddress,addr.BdAddress,sizeof(_admin.ifSettings.macAddress));

    printf("%s - MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
            __FUNCTION__,
            _admin.ifSettings.macAddress[0],
            _admin.ifSettings.macAddress[1],
            _admin.ifSettings.macAddress[2],
            _admin.ifSettings.macAddress[3],
            _admin.ifSettings.macAddress[4],
            _admin.ifSettings.macAddress[5]);

    _admin.ifSettings.MTU = cbBTPAN_getMaxFrameSize();

    minar::Scheduler::postCallback(scheduledConnectPan);
    minar::Scheduler::postCallback(scheduledStartInterfaces);
}

static void scheduledConnectPan()
{
    cb_int32 result;
    result = cbBCM_reqConnectPan(&_napAddr,cbBCM_PAN_ROLE_NAP,cbBCM_PAN_ROLE_PANU,NULL,(cbBCM_ConnectionCallback *)&_connectionCallbacks);
    cb_ASSERT(result == cbBCM_OK);
    printf("%s - connecting over PAN...\n",__FUNCTION__);

}

static void scheduledStartInterfaces()
{
    cbIP_IPv6Settings ipv6settings;

    cbIP_initPanInterfaceDHCP((char*)APP_HOSTNAME, &ipv6settings, &_admin.ifSettings, handleIpStatusIndication, NULL);
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
}

static void initBt(void)
{
    cb_int32 result;
    TCod cod = { { 0, 0, 0 } };

    result = cbBSM_registerCallbacks((cbBSM_Callbacks *)&_securityCallbacks);
    cb_ASSERT(result == cbBSM_OK);

    result = cbBCM_enableServerProfilePan((cb_char*)"ublx-mbed-pan", cbBCM_PAN_ROLE_PANU , (cbBCM_ConnectionCallback *)&_connectionCallbacks);

    result = cbBM_setLocalName((cb_char*)"ublx-odin-mbed");
    cb_ASSERT(result == cbBM_OK);

    result = cbBM_setCod(cod);
    cb_ASSERT(result == cbBM_OK);

    result = cbBM_setMaxConnections(1);
    cb_ASSERT(result == cbBM_OK);

    result = cbBCM_setMaxLinksClassic(1);
    cb_ASSERT(result == cbBCM_OK);

    result = cbBM_setDiscoverableMode(cbBM_DISCOVERABLE_MODE_GENERAL);
    cb_ASSERT(result == cbBM_OK);

    result = cbBM_setConnectableMode(cbBM_CONNECTABLE_MODE_CONNECTABLE);
    cb_ASSERT(result == cbBM_OK);
}

// _securityCallbacks
static void handleRequestPinInd(TBdAddr* pBdAddress)
{
    (void)pBdAddress;
    printf("%s\n",__FUNCTION__);
    /* Ignored, not using pin code for now */
}

static void handleUserConfirmationInd(TBdAddr* pBdAddress, cb_uint32 numericValue)
{
    (void)pBdAddress;
    (void)numericValue;
    printf("%s\n",__FUNCTION__);
    /* Ignored, not using user confirmation for now */
}

static void handleUserPasskeyInd(TBdAddr* pBdAddress)
{
    (void)pBdAddress;
    printf("%s\n",__FUNCTION__);
    /* Ignored, not using passkey for now */
}

static void handleUserPasskeyEvt(TBdAddr* pBdAddress, cb_uint32 passkey)
{
    (void)pBdAddress;
    (void)passkey;
    printf("%s\n",__FUNCTION__);
    /* Ignored, not using passkey for now */
}

// _connectionCallbacks
static void handleConnectInd(cbBCM_Handle handle, cbBCM_ConnectionInfo info)
{
    (void)handle;
    (void)info;
    printf("%s\n",__FUNCTION__);

    /* Ignored, not accepting incoming connections */
}

static void handleConnectEvt(cbBCM_Handle handle, cbBCM_ConnectionInfo info)
{
    (void)handle;
    (void)info;
    printf("%s\n",__FUNCTION__);

    /* Ignored, should not happen since we're not accepting incoming connections */
}

static void handleConnectCnf(cbBCM_Handle handle, cbBCM_ConnectionInfo info, cb_int32 status)
{
    (void)info;

    printf("%s(handle=%lu,status=%lu)\n",__FUNCTION__,handle,status);

    if(status == cbBCM_OK)
    {
        _admin.connHandle = handle;
    }
}

static void handleDisconnectEvt(cbBCM_Handle handle)
{
    printf("%s(%lu)\n",__FUNCTION__,handle);

    _admin.connHandle = cbBCM_INVALID_CONNECTION_HANDLE;
}


void app_start(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    cbMAIN_BtInitParams initParams;

    cbBT_UTILS_setInvalidBdAddr(&initParams.address);
    initParams.leRole = cbBM_LE_ROLE_CENTRAL;
    initParams.maxOutputPower = cbBM_MAX_OUTPUT_POWER;
    initParams.maxLinkKeysClassic = 25;
    initParams.maxLinkKeysLe = 25;

    //GREENTEA_SETUP(60, "default_auto");

    cbMAIN_initBt(&initParams, controllerStartupComplete);

    socket_error_t err = lwipv4_socket_init();
    TEST_EQ(err, SOCKET_ERROR_NONE);

    cbIP_init();

    _timeClient = new UDPTimeClient(SOCKET_STACK_LWIP_IPV4);
}
