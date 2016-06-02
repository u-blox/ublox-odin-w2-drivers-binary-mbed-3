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

/*
* This Bluetooth low energy example does the following:
* 1. Start Bluetooth stack
* 2. Connect to a remote Bluetooth low energy device
* 3. Read u-blox temp value
* 4. Disconnect link
* 5. Wait 10sec
* 6. Goto 2
*/

#include "mbed-drivers/mbed.h"
#include "mbed-drivers/mbed_assert.h"

#include "ublox-odin-w2-drivers/bt_types.h"
#include "ublox-odin-w2-drivers/cb_bt_utils.h"
#include "ublox-odin-w2-drivers/cb_bt_conn_man.h"
#include "ublox-odin-w2-drivers/cb_bt_man.h"
#include "ublox-odin-w2-drivers/cb_bt_sec_man.h"
#include "ublox-odin-w2-drivers/cb_main.h"
#include "ublox-odin-w2-drivers/cb_gatt.h"
#include "ublox-odin-w2-drivers/cb_gatt_client.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define cbGATT_CHAR_UBLOX_TEMP                 0xFFE1
#define RECONNECT_TIME_IN_MS                   10000

#ifndef YOTTA_CFG_TEST_BT_REMOTE_LE_ADDR
#warning YOTTA_CFG_TEST_BT_REMOTE_LE_ADDR must be defined in a config.json file
#define YOTTA_CFG_TEST_BT_REMOTE_LE_ADDR { 0x00,0x12,0xf3,0x1e,0x53,0x7f }, BT_PUBLIC_ADDRESS
#endif

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef enum
{
    STATE_NOT_INITIALIZED,
    STATE_CONNECTING,
    STATE_WAIT_4_READ_TEMP_COMPLETED,
    STATE_DISCONNECTING,
    STATE_DISCONNECTED
} State;

typedef struct
{
    State state;
    TConnHandle aclHandle;
    cbBCM_Handle bcmHandle;
    cb_uint8 appHandle;
    int8 temp;
} Admin;

/*===========================================================================
 * DECLARATIONS
 *=========================================================================*/
static void setState(State state);
static void handleConnectCnf(cbBCM_Handle handle, cbBCM_ConnectionInfo info, cb_int32 status);
static void handleDisconnectEvt(cbBCM_Handle handle);
static void enterReadTemp();
static void enterConnecting();
static void enterDisconnecting();
static void enterDisconnected();
static cb_boolean handleReadCharacteristicByUuidCnf(
    TConnHandle connHandle,
    cbGATT_ErrorCode errorCode,
    cb_uint16 attrHandle,
    cb_uint8* pAttrValue,
    cb_uint16 length,
    cb_boolean moreToRead);

 /*===========================================================================
 * DEFINITIONS
 *=========================================================================*/

static  TBdAddr _remoteAddr = { YOTTA_CFG_TEST_BT_REMOTE_LE_ADDR, BT_PUBLIC_ADDRESS };
static Admin _admin;

static cbBCM_ConnectionCallback _connectionCallbacks =
{
    NULL, //handleConnectInd,
    NULL, //handleConnectEvt,
    handleConnectCnf,
    handleDisconnectEvt
};

static const cbBSM_Callbacks _securityCallbacks =
{
    NULL, //handleRequestPinInd,
    NULL, //handleUserConfirmationInd,
    NULL, //handleUserPasskeyInd,
    NULL, //handleUserPasskeyEvt
    NULL, //handleBondCnf,
    NULL, //handleBondEvt, 
};

static const cbGATT_ClientCallBack _gattClientCallBack =
{
    NULL, //handleGattConnComplEvt,
    NULL, //handleGattDisconnectEvt,
    NULL, //handleDiscoverAllPrimaryServicesCnf,
    NULL, //handleDiscoverAllSecondaryServicesCnf,
    NULL, //handleDiscoverPrimaryServiceByUuidCnf,
    NULL, //handleFindIncludedServicesCnf
    NULL, //handleDiscoverAllCharacteristicsOfServiceCnf,
    NULL, //handleDiscoverAllCharacteristicDescriptorsCnf,
    NULL, //handleReadCharacteristicCnf,
    handleReadCharacteristicByUuidCnf,
    NULL, //handleReadLongCharacteristicCnf,
    NULL, //handleReadMultipleCharacteristicCnf,
    NULL, //handleWriteCharacteristicCnf,
    NULL, //handleWriteCharacteristicNoRspCnf,
    NULL, //handleWriteCharacteristicConfigCnf,
    NULL, //handleWriteLongCharacteristicCnf,
};

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

static void handleConnectCnf(cbBCM_Handle handle, cbBCM_ConnectionInfo info, cb_int32 status)
{
    printf("%s(handle=%lu,status=%ld)\n",__FUNCTION__,handle,status);

    switch (_admin.state)
    {
    case STATE_CONNECTING:
        if (status == cbBCM_OK)
        {
            _admin.aclHandle = info.aclHandle;
            _admin.bcmHandle = handle;
            enterReadTemp();
        }
        else
        {
            printf("Could not connect to remote device, error code=%ld\n",status);
            enterDisconnected();
        }
        break;
    default:
        MBED_ASSERT(0);
        break;
    }
}

static void handleDisconnectEvt(cbBCM_Handle handle)
{
    printf("%s(handle=%lu)\n",__FUNCTION__,handle);
    enterDisconnected();
}

static cb_boolean handleReadCharacteristicByUuidCnf(TConnHandle connHandle,
                                              cbGATT_ErrorCode errorCode,
                                              cb_uint16 attrHandle,
                                              cb_uint8* pAttrValue,
                                              cb_uint16 length,
                                              cb_boolean moreToRead)
{
    printf("%s(connHandle=%u,errorCode=%d,attrHandle=%d,pAttrValue=%d,length=%d,moreToRead=%d)\n",__FUNCTION__,connHandle,errorCode,attrHandle,pAttrValue[0],length,moreToRead);

    MBED_ASSERT(_admin.aclHandle == connHandle);

    switch (_admin.state)
    {
    case STATE_WAIT_4_READ_TEMP_COMPLETED:
        {
            if (errorCode == cbGATT_ERROR_CODE_OK)
            {
                if (length == 1)
                {
                    printf("Remote temp = %d\n", pAttrValue[0]);
                }
                else
                {
                    printf("Invalid length of temp: %d\n", length);
                }
            }
            else
            {
                printf("Could not read temp, error code: %d\n", errorCode);
            }
            enterDisconnecting();
        }
        break;
    default:
        MBED_ASSERT(0);
        break;
    }

    return FALSE; // Stop reading
}

static void setState(State state)
{
    _admin.state = state;

    // Log state change
    if     (state == STATE_NOT_INITIALIZED)             printf("State change to: STATE_NOT_INITIALIZED\n");
    else if(state == STATE_CONNECTING)                  printf("State change to: STATE_CONNECTING\n");
    else if(state == STATE_WAIT_4_READ_TEMP_COMPLETED)  printf("State change to: STATE_WAIT_4_READ_TEMP_COMPLETED\n");
    else if(state == STATE_DISCONNECTING)               printf("State change to: STATE_DISCONNECTING\n");
    else if(state == STATE_DISCONNECTED)                printf("State change to: STATE_DISCONNECTED\n");
    else
    {
        MBED_ASSERT(0);
    }
}

static void enterConnecting()
{
    int16 result;
    cbBCM_ConnectionParametersLe aclParamsLe;
    aclParamsLe.connectionIntervalMax = 20;
    aclParamsLe.connectionIntervalMin = 20;
    aclParamsLe.connectionLatency = 0;
    aclParamsLe.createConnectionTimeout = 5000;
    aclParamsLe.linkLossTimeout = 2000;
    
    result = cbBM_setConnectScanWindow(32);
    MBED_ASSERT(result == cbBM_OK);
    result = cbBM_setConnectScanInterval(32);
    MBED_ASSERT(result == cbBM_OK);
    
    setState(STATE_CONNECTING);
    cbBCM_reqConnectAclLe(&_remoteAddr, &aclParamsLe, &_connectionCallbacks);
}

static void enterReadTemp()
{
    int16 result;
    cbGATT_Uuid ubloxTempUuid = {{cbGATT_CHAR_UBLOX_TEMP},cbGATT_UUID_16};

    setState(STATE_WAIT_4_READ_TEMP_COMPLETED);

    result = cbGATT_readCharacteristicByUuid(_admin.aclHandle,
        cbGATT_MIN_ATTR_HANDLE,
        cbGATT_MAX_ATTR_HANDLE,
        &ubloxTempUuid,
        _admin.appHandle);
    MBED_ASSERT(result == cbGATT_OK);
}

static void enterDisconnecting()
{
    int16 result;
    setState(STATE_DISCONNECTING);
    result = cbBCM_cmdDisconnect(_admin.bcmHandle);
    MBED_ASSERT(result == cbBCM_OK);
}

static void enterDisconnected()
{
    setState(STATE_DISCONNECTED);
    minar::Scheduler::postCallback(enterConnecting).delay(minar::milliseconds(RECONNECT_TIME_IN_MS));
}

static void controllerStartupComplete(void)
{
    switch (_admin.state)
    {
    case STATE_NOT_INITIALIZED:
        {
            int16 result;
            result = cbBSM_registerCallbacks((cbBSM_Callbacks *)&_securityCallbacks);
            MBED_ASSERT(result == cbBSM_OK);

            result = cbBM_setLocalName((cb_char*)"ublx-odin-mbed");
            MBED_ASSERT(result == cbBM_OK);

            result = cbBM_setMaxConnections(1);
            MBED_ASSERT(result == cbBM_OK);

            result = cbBCM_setMaxLinksClassic(0);
            MBED_ASSERT(result == cbBCM_OK);

            result = cbBCM_setMaxLinksLE(1);
            MBED_ASSERT(result == cbBCM_OK);

            result = cbGATT_registerClient(&_gattClientCallBack,&_admin.appHandle);
            MBED_ASSERT(result == cbGATT_OK);

            enterConnecting();
        }
        break;
    default:
        MBED_ASSERT(0);
        break;
    }
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
    cbMAIN_initBt(&initParams, controllerStartupComplete);

    setState(STATE_NOT_INITIALIZED);
}
