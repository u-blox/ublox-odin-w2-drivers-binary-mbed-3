/*
* PackageLicenseDeclared: Apache-2.0
* Copyright (c) 2016 u-blox
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btc_app.h"
#include "ublox-odin-w2-drivers/bt_types.h"
#include "ublox-odin-w2-drivers/cb_bt_utils.h"
#include "ublox-odin-w2-drivers/cb_bt_serial.h"
#include "ublox-odin-w2-drivers/cb_bt_man.h"
#include "ublox-odin-w2-drivers/cb_bt_conn_man.h"
#include "ublox-odin-w2-drivers/cb_bt_sec_man.h"

/*===========================================================================
* DEFINES
*=========================================================================*/

/*===========================================================================
* TYPES
*=========================================================================*/

typedef enum
{
    State_Idle = 0,
    State_Inquiring,
    State_InquiryCompleted,
    State_Bonding,
    State_bondingCompleted,
    State_ConnectingSPP,
    State_SPPConnected
}btcAPP_State;

typedef enum
{
    SPProle_Unknown = 0,
    SPProle_Client,
    SPProle_Server
}btcAPP_SPP_Role;

typedef enum
{
    State_Connecting = 0,
    State_Disconnecting,
    State_Connected,
    State_SerialOpen,
    State_Disconnected
}btcAPP_Conn_State;

typedef struct
{
    TBdAddr                  bdAddr;
    int32_t                  serverChannel;
    btcAPP_Conn_State        connectedState;
    btcAPP_SPP_Role          role;
    cbBCM_Handle             connHandle;
} btcAPP_Connection;

struct listNode
{
    TBdAddr             bdAddr;
    struct listNode     *next;
};

struct devListNode
{
    btcAPP_Connection      *deviceInfo;
    struct devListNode     *next;
};

typedef struct
{
    btcAPP_State            app_state;
    btcAPP_Conn_State       conn_state;
    struct devListNode     *devicesList;
    struct listNode         *foundDevicesList;
    uint16_t                nbrOfFoundDevices;
    btcAPP_EventCallBack*   eventCallback;
} btcAppContainer;

/*===========================================================================
* DECLARATIONS
*=========================================================================*/

static void inquiryEventCallback(
    TBdAddr     *pBdAddress,
    TCod        cod,
    uint16_t   clockOffset,
    cb_int8     rssi,
    cb_char     *pName,
    TExtInqRsp* pExtInqRsp,
    cb_uint8    length);

static void inquiryCompleteCallback(cb_int32 status);

static void HandleRequestPinInd(TBdAddr* pBdAddress);
static void HandleUserConfirmationInd(TBdAddr* pBdAddress, cb_uint32 numericValue);
static void HandleUserPasskeyInd(TBdAddr* pBdAddress);
static void HandleUserPasskeyEvt(TBdAddr* pBdAddress, cb_uint32 passkey);
static void HandleBondCnf(cbBSM_BondStatus status, TBdAddr* pBdAddress);

static void BTStreamDataAvailable(cbBCM_Handle handle);
static void BTStreamWriteCnf(cbBCM_Handle handle, cb_int32 status, cb_uint32 nBytes, cb_int32 tag);

static void HandleConnectInd(cbBCM_Handle handle, cbBCM_ConnectionInfo info);
static void HandleConnectEvt(cbBCM_Handle handle, cbBCM_ConnectionInfo info);
static void HandleConnectCnf(cbBCM_Handle handle, cbBCM_ConnectionInfo info, cb_int32 status);
static void HandleDisconnectEvt(cbBCM_Handle handle);

/*===========================================================================
* DEFINITIONS
*=========================================================================*/

static btcAppContainer btcAPP;

static const cbBSM_Callbacks SecurityCallbacks =
{
    HandleRequestPinInd,
    HandleUserConfirmationInd,
    HandleUserPasskeyInd,
    HandleUserPasskeyEvt,
    HandleBondCnf,
    HandleBondCnf
};

static const cbBCM_ConnectionCallback ConnectionCallbacks =
{
    HandleConnectInd,
    HandleConnectEvt,
    HandleConnectCnf,
    HandleDisconnectEvt
};

static const cbBSE_Callback BTStreamCallbacks =
{
    BTStreamDataAvailable,
    BTStreamWriteCnf
};

/*===========================================================================
* INTERNAL FUNCTIONS
*=========================================================================*/

static void foundDevListDeleteAll(void)
{
    struct listNode *node;
    while (btcAPP.foundDevicesList != NULL)
    {
        node = btcAPP.foundDevicesList;
        btcAPP.foundDevicesList = btcAPP.foundDevicesList->next;
        free(node);
    }
}

static btcAPPe foundDevListAdd(TBdAddr *pBDAddr)
{
    btcAPPe result = BTC_APP_ERROR;
    struct listNode *node = malloc(sizeof(struct listNode));
    
    if (node != NULL)
    {
        memcpy((void*)&(node->bdAddr), (void*)pBDAddr, sizeof(TBdAddr));
        if (btcAPP.foundDevicesList == NULL)
        {
            btcAPP.foundDevicesList = node;
            node->next = NULL;
        }
        else
        {
            node->next = btcAPP.foundDevicesList;
            btcAPP.foundDevicesList = node;
        }
        result = BTC_APP_OK;
    }
    return result;
}

//static void deviceListDeleteAll(void)
//{
//    struct devListNode *node;
//    while (btcAPP.devicesList != NULL)
//    {
//        node = btcAPP.devicesList;
//        btcAPP.devicesList = btcAPP.devicesList->next;
//        free(node);
//    }
//}

static btcAPPe deviceListAdd(btcAPP_Connection *pDevice)
{
    btcAPPe result = BTC_APP_ERROR;
    struct devListNode *node = malloc(sizeof(struct devListNode));

    if (node != NULL)
    {
        node->deviceInfo = malloc(sizeof(btcAPP_Connection));
        if (node->deviceInfo != NULL)
        {
            memcpy((void*)(node->deviceInfo), (void*)pDevice, sizeof(btcAPP_Connection));
            if (btcAPP.devicesList == NULL)
            {
                btcAPP.devicesList = node;
                node->next = NULL;
            }
            else
            {
                node->next = btcAPP.devicesList;
                btcAPP.devicesList = node;
            }
            result = BTC_APP_OK;
        }
    }
    return result;
}

static void deviceListDelete(TBdAddr *pBDAddr)
{
    struct devListNode *temp, *prev;

    temp = btcAPP.devicesList;
    prev = temp;

    while (temp != NULL)
    {
        if (0 == memcmp((void*)&(temp->deviceInfo->bdAddr.BdAddress), (void*)pBDAddr->BdAddress, SIZE_OF_BD_ADDR))
        {
            if (temp == btcAPP.devicesList)
            {
                btcAPP.devicesList = temp->next;
                free(temp);
                return;
            }
            else
            {
                prev->next = temp->next;
                free(temp);
                return;
            }
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
}

// if pBDAddr is NULL will search by handle
static btcAPP_Connection *deviceListFind(TBdAddr *pBDAddr, cbBCM_Handle handle)
{
    struct devListNode *temp = btcAPP.devicesList;

    while (temp != NULL)
    {
        if (pBDAddr != NULL)
        {
            if (0 == memcmp((void*)(temp->deviceInfo->bdAddr.BdAddress), (void*)pBDAddr->BdAddress, SIZE_OF_BD_ADDR))
            {
                return (temp->deviceInfo);
            }
        }
        else if (handle == temp->deviceInfo->connHandle)
        {
            return (temp->deviceInfo);
        }
        temp = temp->next;
    }
    return NULL;
}

//static bool getConHandleByAddress(TBdAddr* pBdAddress, int *connHandle)
//{
//    btcAPP_Connection* pDevice;
//    bool found = FALSE;
//
//    pDevice = deviceListFind(pBdAddress, 0);
//    if (pDevice != NULL)
//    {
//        *connHandle = pDevice->connHandle;
//        found = TRUE;
//    }
//    return found;
//}

static int devieListCount(void)
{
    struct devListNode *n;
    int c = 0;
    n = btcAPP.devicesList;
    while (n != NULL)
    {
        n = n->next;
        c++;
    }
    return c;
}

// SecurityCallbacks
static void HandleRequestPinInd(TBdAddr* pBdAddress)
{
    (void)pBdAddress;
}

static void HandleUserConfirmationInd(TBdAddr* pBdAddress, cb_uint32 numericValue)
{
    (void)pBdAddress;
    (void)numericValue;
}

static void HandleUserPasskeyInd(TBdAddr* pBdAddress)
{
    (void)pBdAddress;
}

static void HandleUserPasskeyEvt(TBdAddr* pBdAddress, cb_uint32 passkey)
{
    (void)pBdAddress;
    (void)passkey;
}

static void HandleBondCnf(cbBSM_BondStatus status, TBdAddr* pBdAddress)
{
    printf("%s status:%d %02X:%02X:%02X:%02X:%02X:%02X  \n", __FUNCTION__, status, pBdAddress->BdAddress[0],
                                                                                   pBdAddress->BdAddress[1],
                                                                                   pBdAddress->BdAddress[2],
                                                                                   pBdAddress->BdAddress[3],
                                                                                   pBdAddress->BdAddress[4],
                                                                                   pBdAddress->BdAddress[5]);
    if (btcAPP.eventCallback != NULL)
    {
        btcAPP.eventCallback->bondEvent(status);
    }
}

// Stream
static void BTStreamDataAvailable(cbBCM_Handle handle)
{
    int32_t result;
    uint8_t* pData;
    uint32_t len;

    if (btcAPP.eventCallback != NULL)
    {
        btcAPP.eventCallback->dataAvailableEvt(handle);
    }
    else
    {
        do
        {
            result = cbBSE_getReadBuf(handle, &pData, &len);
            if (result == BTC_APP_OK)
            {
                cbBSE_readBufConsumed(handle, len);
            }
        } while (result == BTC_APP_OK);
    }
}
static void BTStreamWriteCnf(cbBCM_Handle handle, cb_int32 status, cb_uint32 nBytes, cb_int32 tag)
{
    if (btcAPP.eventCallback != NULL)
    {
        btcAPP.eventCallback->streamWriteCnf(handle, status, nBytes, tag);
    }
}

// ConnectionCallbacks
static void HandleConnectInd(cbBCM_Handle handle, cbBCM_ConnectionInfo info)
{
    (void)handle;
    (void)info;
}

static void HandleConnectEvt(cbBCM_Handle handle, cbBCM_ConnectionInfo info)
{
    (void)handle;
    (void)info;
}

static void HandleConnectCnf(cbBCM_Handle handle, cbBCM_ConnectionInfo info, cb_int32 status)
{
    btcAPP_Connection* pDevice = NULL;
    cb_int32 result = BTC_APP_ERROR;

    pDevice = deviceListFind(&info.address, 0);
    printf("HandleConnectCnf handle:%ld, status=%ld \n", handle, status);

    if ((status == cbBCM_OK) && (pDevice != NULL))
    {
        pDevice->serverChannel = info.serverChannel;
        pDevice->connHandle = handle;
        pDevice->connectedState = State_Connected;
        btcAPP.conn_state = State_Connected;

        result = cbBSE_open(handle, (cbBSE_Callback *)&BTStreamCallbacks);
        printf("cbBSE_open handle:%ld, result=%ld\n", handle, result);
        if (btcAPP.eventCallback != NULL)
        {
            btcAPP.eventCallback->connectEvt(result, handle, &info.address);
        }
        //todo: if failed disconnect SPP
    }
    else if (btcAPP.eventCallback != NULL)
    {
        deviceListDelete(&(pDevice->bdAddr));
        btcAPP.eventCallback->connectEvt(result, handle, &info.address);
    }
}

static void HandleDisconnectEvt(cbBCM_Handle handle)
{
    btcAPP_Connection *conn = deviceListFind(NULL, handle);

    if (conn != NULL)
    {
        if (btcAPP.eventCallback != NULL)
        {
            btcAPP.eventCallback->disonnectEvt(handle, &(conn->bdAddr));
        }
        deviceListDelete(&(conn->bdAddr));
    }
}

// Inquiry
static void inquiryEventCallback(
    TBdAddr     *pBdAddress,
    TCod        cod,
    uint16_t   clockOffset,
    cb_int8     rssi,
    cb_char     *pName,
    TExtInqRsp* pExtInqRsp,
    cb_uint8    length)
{
    (void)cod;
    (void)clockOffset;
    (void)rssi;
    (void)pExtInqRsp;
    (void)length;
    printf("%s: name=%s (%02X:%02X:%02X:%02X:%02X:%02X) \n", __FUNCTION__, pName, pBdAddress->BdAddress[0],
                                                                                  pBdAddress->BdAddress[1],
                                                                                  pBdAddress->BdAddress[2],
                                                                                  pBdAddress->BdAddress[3],
                                                                                  pBdAddress->BdAddress[4],
                                                                                  pBdAddress->BdAddress[5]);
    if (foundDevListAdd(pBdAddress) == BTC_APP_OK)
    {
        btcAPP.nbrOfFoundDevices++;
    }
}

static void inquiryCompleteCallback(cb_int32 status)
{
    (void)status;
    if (btcAPP.eventCallback != NULL)
    {
        btcAPP.eventCallback->inquiryCnf(status, btcAPP.nbrOfFoundDevices);
    }
    btcAPP.app_state = State_Idle;
}

/*===========================================================================
* EXPORTED FUNCTIONS
*=========================================================================*/

void btcAPPinit(void)
{
    btcAPPe result = BTC_APP_OK;
    cb_uint8 channel;
    cb_char readname[40];
    TCod cod = { { 1, 2, 3 } };
    TBdAddr Address;

    btcAPP.eventCallback = NULL;

    //register callbacks
    result = cbBSM_registerCallbacks((cbBSM_Callbacks *)&SecurityCallbacks);

    result = cbBCM_enableServerProfileSpp("ublx-mbed-spp", &channel, (cbBCM_ConnectionCallback *)&ConnectionCallbacks);

    result = cbBM_setLocalName("ublx-odin-mbed");

    result = cbBM_setCod(cod);

    result = cbBM_setMaxConnections(7);
    result = cbBCM_setMaxLinksClassic(7);

    result = cbBM_getMaxTxPower();
    printf("\n Max Tx Power = %ld \n",result);

    result = cbBM_setDiscoverableMode(cbBM_DISCOVERABLE_MODE_GENERAL);

    result = cbBM_setConnectableMode(cbBM_CONNECTABLE_MODE_CONNECTABLE);

    // test :
    result = cbBM_getLocalName(readname, 39);
    printf("\n cbBM_getLocalName result:%ld name:  %s \n", result, readname);
    result = cbBM_getLocalAddress(&Address);
    printf("\n cbBM_getLocalAddress result:%ld ADR:%02X:%02X:%02X:%02X:%02X:%02X", result, Address.BdAddress[0],
                                                                                           Address.BdAddress[1],
                                                                                           Address.BdAddress[2],
                                                                                           Address.BdAddress[3],
                                                                                           Address.BdAddress[4],
                                                                                           Address.BdAddress[5]);
    btcAPP.app_state = State_Idle;
    btcAPP.conn_state = State_Disconnected;
    btcAPP.nbrOfFoundDevices = 0;
    btcAPP.foundDevicesList = NULL;
    btcAPP.devicesList = NULL;
}

btcAPPe btcAPPregisterCallBacks(btcAPP_EventCallBack* pCallBacks)
{
    btcAPPe result = BTC_APP_ERROR;

    if (!btcAPP.eventCallback && pCallBacks)
    {
        btcAPP.eventCallback = pCallBacks;
        result = BTC_APP_OK;
    }
    return result;
}

btcAPPe btcAPPinquiry(void)
{
    btcAPPe result = BTC_APP_ERROR;

    if (cbBM_OK == cbBM_inquiry(cbBM_INQUIRY_GENERAL, 5000, inquiryEventCallback, inquiryCompleteCallback))
    {
        result = BTC_APP_OK;
        btcAPP.app_state = State_Inquiring;
    }
    foundDevListDeleteAll();
    btcAPP.nbrOfFoundDevices = 0;
    return result;
}

btcAPPe btcAPPreqBond(TBdAddr *pAddress)
{
    btcAPPe result = BTC_APP_ERROR;

    result = cbBSM_reqBond(*pAddress, BT_TYPE_CLASSIC);
    printf("cbBSM_reqBond, result=%ld \n", result);

    return result;
}

btcAPPe btcAPPrequestConnectSPP(TBdAddr *pAddress)
{
    btcAPPe result = BTC_APP_ERROR;
    btcAPP_Connection device;

    if (btcAPP_MAX_CONNECTIONS == devieListCount())
    {
        // max number of connections reached
        return result;
    }

    device.connHandle = cbBCM_reqConnectSpp(pAddress, NULL, cbBCM_INVALID_SERVER_CHANNEL, NULL, (cbBCM_ConnectionCallback*)&ConnectionCallbacks);

    if (device.connHandle != cbBCM_INVALID_CONNECTION_HANDLE)
    {
        printf("SPP connect success \n");
        memcpy((void*)&(device.bdAddr), (void*)(pAddress), sizeof(TBdAddr));
        device.connectedState = State_Connecting;
        device.role = SPProle_Client;
        device.serverChannel = -1;
        btcAPP.conn_state = State_Connecting;
        deviceListAdd(&device);
        result = BTC_APP_OK;
    }
    return result;
}

btcAPPe btcAPPwrite(uint32_t handle, uint8_t *pBuf, uint32_t nBytes, int32_t tag)
{
    int32_t result = BTC_APP_ERROR;
    btcAPP_Connection *conn = deviceListFind(NULL, handle);

    if (conn != NULL)
    {
        result = cbBSE_write(handle, pBuf, nBytes, tag);
    }
    return result;
}

btcAPPe btcAPPdisconnect(uint32_t handle)
{
    int32_t result = BTC_APP_ERROR;
    btcAPP_Connection *conn = deviceListFind(NULL, handle);

    if (conn != NULL)
    {
        result = cbBCM_cmdDisconnect(handle);
    }
    return result;
}