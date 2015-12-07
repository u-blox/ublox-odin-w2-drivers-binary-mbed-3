/*
 * Copyright (c) 2012-2015, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WLANINTERFACE_H__
#define __WLANINTERFACE_H__

//#if !defined(TARGET_UBLOX_ODIN_W2)
//#error The Wlan Interface library is not supported on this target
//#endif

#include "mbed-drivers/platform.h"
#include "mbed-drivers/cthunk.h"

 /** Interface using Wlan to connect to an IP-based network
 *
 */
  typedef void(*InitializedEventCallback_t)(void);
  typedef void(*ConnectionEventCallback_t)(void);
  typedef void(*DisconnectionEventCallback_t)(void);
  typedef void(*NetworkUpEventCallback_t)(void);

class WlanInterface {
public:
private:
//    void *initializedCallbackP;
public:

    WlanInterface();


    //static void staticMemberFn();
  InitializedEventCallback_t initializedCallback;
  ConnectionEventCallback_t connectionCallback;
  DisconnectionEventCallback_t disconnectionCallback;
  NetworkUpEventCallback_t networkUpCallback;

  void onInitialized(InitializedEventCallback_t callback) { initializedCallback = callback; }
  void onConnection(ConnectionEventCallback_t callback) { connectionCallback = callback; }
  void onDisconnection(DisconnectionEventCallback_t callback) { disconnectionCallback = callback; }
  void onNetworkUp(NetworkUpEventCallback_t callback) { networkUpCallback = callback; }

    //static void handleWlanManagerInitEvent();

  /** Initialize the interface with DHCP.
  * Initialize the interface and configure it to use DHCP (no connection at this point).
  * \return 0 on success, a negative number on failure
  */
  int init(); //With DHCP

  /** Initialize the interface with a static IP address.
  * Initialize the interface and configure it with the following static configuration (no connection at this point).
  * \param ip the IP address to use
  * \param mask the IP address mask
  * \param gateway the gateway to use
  * \return 0 on success, a negative number on failure
  */
  int init(const char* ip, const char* mask, const char* gateway);

  /** Connect WPA2
  * Bring the interface up, start DHCP if needed.
  * \return 0 on success, a negative number on failure
  */
  void connectWpa2(char* ssid, char* passphrase);

  /** Connect Open
  * Bring the interface up, start DHCP if needed.
  * \return 0 on success, a negative number on failure
  */
  void connectOpen(char* ssid);

  /** Disconnect
  * Bring the interface down
  * \return 0 on success, a negative number on failure
  */
  int disconnect();

  /** Get the MAC address of your Wlan interface
   * \return a pointer to a string containing the MAC address
   */
  char* getMACAddress();

  /** Get the IP address of your Wlan interface
   * \return a pointer to a string containing the IP address
   */
  char* getIPAddress();

  /** Get the Gateway address of your Wlan interface
   * \return a pointer to a string containing the Gateway address
   */
  char* getGateway();

  /** Get the Network mask of your Wlan interface
   * \return a pointer to a string containing the Network mask
   */
  char* getNetworkMask();

  protected:
      //err_t eth_arch_enetif_init(struct netif* netif);
      void started_handler(void);
      void connected_handler(void);
      void disconnected_handler(void);
      void network_up_handler(void);
      CThunk<WlanInterface> _startedCThunk;
      CThunk<WlanInterface> _connectedCThunk;
      CThunk<WlanInterface> _disconnectedCThunk;
      CThunk<WlanInterface> _networkUpCThunk;
};


#endif /* __WLANINTERFACE_H__ */
