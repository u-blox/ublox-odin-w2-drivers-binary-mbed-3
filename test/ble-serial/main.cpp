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
* 1. Start Bluetooth stack as BLE peripheral
* 2. Start advertising
* 3. Start u-blox serial port service
* 4. Accept incoming connections
* 5. Print received ascii data
*/

#include "mbed-drivers/mbed.h"
#include "mbed-drivers/mbed_assert.h"

#include "ublox-odin-w2-drivers/bt_types.h"
#include "ublox-odin-w2-drivers/cb_bt_utils.h"
#include "ublox-odin-w2-drivers/cb_bt_conn_man.h"
#include "ublox-odin-w2-drivers/cb_bt_man.h"
#include "ublox-odin-w2-drivers/cb_bt_sec_man.h"
#include "ublox-odin-w2-drivers/cb_bt_serial_le.h"
#include "ublox-odin-w2-drivers/cb_main.h"
#include "stdlib.h"
#include "string.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef enum
{
    STATE_NOT_INITIALIZED,
    STATE_SPS_NOT_CONNECTED,
    STATE_SPS_CONNECTED,
} State;

typedef struct
{
    State state;
    cbBCM_Handle connHandle;
} Admin;

/*===========================================================================
 * DECLARATIONS
 *=========================================================================*/
static void setState(State state);

static void handleConnectInd(cbBCM_Handle handle, cbBCM_ConnectionInfo info);
static void handleConnectEvt(cbBCM_Handle handle, cbBCM_ConnectionInfo info);
static void handleDisconnectEvt(cbBCM_Handle handle);
static void handleDataAvailEvt(cbBCM_Handle handle);
static void handleWriteCnf(cbBCM_Handle handle, cb_int32 status, cb_uint32 nBytes, cb_int32 tag);

/*===========================================================================
 * DEFINITIONS
 *=========================================================================*/

static Admin _admin;

static const cbBSM_Callbacks _securityCallbacks =
{
    NULL, //handleRequestPinInd,
    NULL, //handleUserConfirmationInd,
    NULL, //handleUserPasskeyInd,
    NULL, //handleUserPasskeyEvt,
    NULL, //handleBondCnf,
    NULL, //handleBondEvt, 
};

static cbBCM_ConnectionCallback _connectionCallbacks =
{
    handleConnectInd,
    handleConnectEvt,
    NULL, //handleConnectCnf,
    handleDisconnectEvt
};

static cbBSL_Callback _spsDataCallback = 
{
    handleDataAvailEvt,
    handleWriteCnf
};

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

static void setState(State state)
{
    _admin.state = state;

    // Log state change
    if     (state == STATE_NOT_INITIALIZED)         printf("State change to: STATE_NOT_INITIALIZED\n");
    else if(state == STATE_SPS_NOT_CONNECTED)           printf("State change to: STATE_SPS_NOT_CONNECTED\n");
    else if(state == STATE_SPS_CONNECTED)               printf("State change to: STATE_SPS_CONNECTED\n");
    else
    {
        MBED_ASSERT(0);
    }
}
     
static void handleConnectInd(cbBCM_Handle handle, cbBCM_ConnectionInfo info)
{
    (void)handle;
    (void)info;
    cb_int32 result;

    printf("%s\n", __FUNCTION__);

    if (info.type == cbBCM_SPS_CONNECTION)
    {
        result = cbBCM_rspConnectSpsCnf(handle, TRUE); // Always accept incoming connections on SPS
        MBED_ASSERT(result == cbBCM_OK);
    }
    else
    {
        result = cbBCM_rspConnectSpsCnf(handle, FALSE);
        MBED_ASSERT(result == cbBCM_OK);
    }
}

static void handleConnectEvt(cbBCM_Handle handle, cbBCM_ConnectionInfo info)
{
    (void)handle;
    (void)info;
    cb_int32 result;

    printf("%s\n",__FUNCTION__);

    setState(STATE_SPS_CONNECTED);
    
    MBED_ASSERT(info.type == cbBCM_SPS_CONNECTION);

    result = cbBSL_open(handle, &_spsDataCallback);
    MBED_ASSERT(result == cbBSL_OK);
}

static void handleDisconnectEvt(cbBCM_Handle handle)
{
    (void)handle;
    printf("%s\n",__FUNCTION__);

    setState(STATE_SPS_NOT_CONNECTED);
    _admin.connHandle = cbBCM_INVALID_CONNECTION_HANDLE;
}


static void handleDataAvailEvt(cbBCM_Handle handle)
{
    cb_int32 result;
    cb_uint8* pData;
    cb_uint32 length;

    result = cbBSL_getReadBuf(handle, &pData, &length);
    MBED_ASSERT(result == cbBSL_OK);

    // Assume that the data is printable i.e. ascii data,
    cb_uint8* tmpData = (cb_uint8*)malloc(length + 1);
    memcpy(tmpData,pData,length);
    // Add zero termination for string
    tmpData[length] = 0;

    printf("Received data: %s\n",tmpData);

    // Consume all bytes so we can receive more
    result = cbBSL_readBufConsumed(handle,length);
    MBED_ASSERT(result == cbBSL_OK);
    free(tmpData);
}

static void handleWriteCnf(cbBCM_Handle handle, cb_int32 status, cb_uint32 nBytes, cb_int32 tag)
{
    (void)handle;
    (void)status;
    (void)nBytes;
    (void)tag;
    // Nothing for now
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

            result = cbBM_setAdvertisingIntervalMin(100);
            MBED_ASSERT(result == cbBM_OK);

            result = cbBM_setAdvertisingIntervalMax(100);
            MBED_ASSERT(result == cbBM_OK);

            result = cbBM_setConnectableModeLe(cbBM_CONNECTABLE_MODE_LE_CONNECTABLE);
            MBED_ASSERT(result == cbBM_OK);

            result = cbBM_setDiscoverableModeLe(cbBM_DISCOVERABLE_MODE_LE_GENERAL);
            MBED_ASSERT(result == cbBM_OK);

            result = cbBCM_enableDevInfoService("My manufacturer", "My model", "0.0.1", 1024);
            MBED_ASSERT(result == cbBCM_OK);

            result = cbBCM_enableSps(&_connectionCallbacks);
            MBED_ASSERT(result == cbBCM_OK);

            setState(STATE_SPS_NOT_CONNECTED);
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

    cbBM_InitParams initParams;
    cbBT_UTILS_setInvalidBdAddr(&initParams.address);
    initParams.leRole = cbBM_LE_ROLE_PERIPHERAL;
    initParams.maxOutputPower = cbBM_MAX_OUTPUT_POWER;
    initParams.nvdsStartIdLinkKeysClassic = 1000 + 2300;
    initParams.maxLinkKeysClassic = 25;
    initParams.nvdsStartIdLinkKeysLe = 1000 + 2500;
    initParams.maxLinkKeysLe = 25;
    cbMAIN_initBt(&initParams, controllerStartupComplete);

    setState(STATE_NOT_INITIALIZED);
}
