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
 * - Initialize Ethernet driver
 * - Start lwIP
 * - Set static IP address
 * - Start echo server on port 5555
 *
 *   The code below mixes C and C++. The reason for this is that the 
 *   Ethernet interface is purely C while the UDP socket is C++.
 *  
 *   TODO: Implement notifications for cable unplug
 *   TODO: Remove interface and disconnect after receiving the time
 *   TODO: Add minimal state machine
 */

#include <stdio.h>
#include <string.h>

#include "greentea-client/test_env.h"
#include "mbed-drivers/mbed.h"
#include "mbed-drivers/mbed_assert.h"
#include "sockets/TCPStream.h"
#include "sal/test/ctest_env.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "sockets/UDPSocket.h"

#include "sal-stack-lwip-ublox-odin-w2/lwipv4_init.h"
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
#define APP_ECHO_SRV_PORT 5555
#define UDP_ECHO_CLIENT_BUFFER_SIZE 2000

/*===========================================================================
* TYPES
*=========================================================================*/

typedef struct
{
    bool interfaceUp;
    cbIP_interfaceSettings ifSettings;
    cbIP_IPv4Settings currentIPv4Settings;
    cbIP_IPv6Settings currentIPv6Settings;
    cbIP_IPv4Address resolvedAddr;
} APP_admin;

class UDPEchoServer;
typedef mbed::util::FunctionPointer2<void, bool, UDPEchoServer*> fpterminate_t;

/*===========================================================================
* DECLARATIONS
*=========================================================================*/

static void terminate(bool status, UDPEchoServer* client);
static void handleIpStatusIndication(
    cbIP_Status status,
    void* callbackArg,
    cbIP_IPv4Settings const * const ipV4settings,
    cbIP_IPv6Settings const * const ipV6settings);

/*===========================================================================
* DEFINITIONS
*=========================================================================*/
UDPEchoServer *_udpEchoServer;
static APP_admin _admin;

/*===========================================================================
* FUNCTIONS/CLASSES
*=========================================================================*/

class UDPEchoServer {
public:
    UDPEchoServer(socket_stack_t stack) :
        _usock(stack)
    {
        _usock.setOnError(UDPSocket::ErrorHandler_t(this, &UDPEchoServer::onError));
    }
    void start(uint16_t port)
    {
        _port = port;

        socket_error_t err = _usock.open(SOCKET_AF_INET4);
        TEST_EQ(err, SOCKET_ERROR_NONE);

        _usock.setOnReadable(UDPSocket::ReadableHandler_t(this,&UDPEchoServer::onRx));

        err = _usock.bind("0.0.0.0", APP_ECHO_SRV_PORT);
        if(!TEST_EQ(err, SOCKET_ERROR_NONE)) {
            printf("MBED: Failed to bind \r\n");
            onError(&_usock, err);
        }
        printf("UDP echo server started at port %d\n",APP_ECHO_SRV_PORT);
    }
    void onError(Socket *s, socket_error_t err) {
        (void) s;
        printf("MBED: Socket Error: %s (%d)\r\n", socket_strerror(err), err);
        minar::Scheduler::postCallback(fpterminate_t(terminate).bind(TEST_RESULT(),this));
    }
    void onRx(Socket *s)
    {
        SocketAddr addr;
        uint16_t port;
        (void)s;
        unsigned int n = sizeof(out_buffer);
        socket_error_t err = _usock.recv_from(out_buffer, &n, &addr, &port);
        TEST_EQ(err, SOCKET_ERROR_NONE);

        err = _usock.send_to(out_buffer, n, &addr, port);
        TEST_EQ(err, SOCKET_ERROR_NONE);
        
        //minar::Scheduler::postCallback(fpterminate_t(terminate).bind(TEST_RESULT(),this));
    }
    bool isError() {
        return !TEST_RESULT();
    }

protected:
    UDPSocket _usock;
    SocketAddr _resolvedAddr;
    uint16_t _port;
    char out_buffer[UDP_ECHO_CLIENT_BUFFER_SIZE];
};

static void terminate(bool status, UDPEchoServer* client)
{
    delete client;
    GREENTEA_TESTSUITE_RESULT(status);
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

            mbed::util::FunctionPointer1<void, uint16_t> fp(_udpEchoServer, &UDPEchoServer::start);
            minar::Scheduler::postCallback(fp.bind(APP_ECHO_SRV_PORT));
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

    cbIP_IPv4Settings ipv4Settings;
    cbIP_SET_IP4_ADDR(ipv4Settings.address, 192, 168, 0, 2);
    cbIP_SET_IP4_ADDR(ipv4Settings.dns0, 192, 168, 0, 1);
    cbIP_SET_IP4_ADDR(ipv4Settings.dns1, 192, 168, 0, 1);
    cbIP_SET_IP4_ADDR(ipv4Settings.gateway, 192, 168, 0, 1);
    cbIP_SET_IP4_ADDR(ipv4Settings.netmask, 255, 255, 255, 0);
    
	cb_uint8* macAddr = cbMAIN_initEth(NULL);

    //GREENTEA_SETUP(60, "default_auto");

    socket_error_t err = lwipv4_socket_init();
    MBED_ASSERT(err == SOCKET_ERROR_NONE);

    cbIP_init();

    _udpEchoServer = new UDPEchoServer(SOCKET_STACK_LWIP_IPV4);

    cbIP_interfaceSettings ifConfig;
    memset(&ifConfig, 0x00, sizeof(ifConfig));
	memcpy(ifConfig.macAddress,macAddr,6);
    ifConfig.MTU = 1500;
    cbIP_initEthInterfaceStatic((char*)APP_HOSTNAME, &ipv4Settings, &ifConfig, handleIpStatusIndication);
}
