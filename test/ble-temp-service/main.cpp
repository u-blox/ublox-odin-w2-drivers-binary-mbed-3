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
* 3. Start device info service
* 4. Accept connections
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
#include "ublox-odin-w2-drivers/cb_gatt_server.h"

#include "cb_temp_service.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef enum
{
    STATE_NOT_INITIALIZED,
    STATE_NOT_CONNECTED,
    STATE_CONNECTED,
} State;

typedef struct
{
	State state;
    cb_uint8 appHandle;
    int8 temp;
} Admin;

/*===========================================================================
 * DECLARATIONS
 *=========================================================================*/
 static void setState(State state);

static void handleConnComplEvt(
    TConnHandle         handle, 
    TErrorCode          errorCode, 
    cb_uint8            role, 
    TBdAddr             peerBdAddress, 
    cb_uint16           connInterval,
    cb_uint16           connLatency,
    cb_uint16           connTmo,
    cb_uint8            masterClkAccuracy);
     
static void handleDisconnectEvt(
    TConnHandle         handle, 
    TErrorCode          errorCode);

 /*===========================================================================
 * DEFINITIONS
 *=========================================================================*/

static Admin _admin;

static const cbBSM_Callbacks _securityCallbacks =
{
    NULL, //handleRequestPinInd,
    NULL, //handleUserConfirmationInd,
    NULL, //handleUserPasskeyInd,
    NULL, //handleUserPasskeyEvt
};

static const cbGATT_ServerCallBack _gattServerCallBack =
{
    handleConnComplEvt,
    handleDisconnectEvt,
    NULL, //handleCharacteristicValueIndicationCnf,
    NULL, //handleCharacteristicValueNotificationCnf,
    NULL, //handleCharacteristicWriteLongCommitEvt
};

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

static void setState(State state)
{
    _admin.state = state;

    // Log state change
    if     (state == STATE_NOT_INITIALIZED)         printf("State change to: STATE_NOT_INITIALIZED\n");
    else if(state == STATE_NOT_CONNECTED)           printf("State change to: STATE_NOT_CONNECTED\n");
    else if(state == STATE_CONNECTED)               printf("State change to: STATE_CONNECTED\n");
    else
    {
        MBED_ASSERT(0);
    }
}

static void handleConnComplEvt(
    TConnHandle         handle, 
	TErrorCode          errorCode, 
	cb_uint8            role, 
	TBdAddr             peerBdAddress, 
	cb_uint16           connInterval,
	cb_uint16           connLatency,
	cb_uint16           connTmo,
	cb_uint8            masterClkAccuracy)
{
	(void)handle;
	(void)role;
	(void)peerBdAddress;
	(void)connInterval;
	(void)connLatency;
	(void)connTmo;
	(void)masterClkAccuracy;

	if (errorCode == cbGATT_ERROR_CODE_OK)
	{
		setState(STATE_CONNECTED);
	}
	else
	{
		printf("Failed connection, error code: %d\n",errorCode);
	}
}
     
static void handleDisconnectEvt(
    TConnHandle         handle, 
	TErrorCode          errorCode)
{
	(void)handle;

	setState(STATE_NOT_CONNECTED);
    printf("Disconnection, error code: %d\n",errorCode);
}

static void increaseTemp()
{
	static cb_uint8 temp = 0;
	temp++;
	cbTEMP_SERVICE_set(temp);
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

            result = cbGATT_registerServer(&_gattServerCallBack,&_admin.appHandle);
            MBED_ASSERT(result == cbGATT_OK);

	        result = cbBCM_enableDevInfoService("My manufacturer", "My model", "0.0.1", 1024);
            MBED_ASSERT(result == cbBCM_OK);

	        cbTEMP_SERVICE_init();
          	result = cbTEMP_SERVICE_register(2048);
            MBED_ASSERT(result == cbTEMP_SERVICE_OK);

            // Fake the temp by increasing the value every second
            minar::Scheduler::postCallback(increaseTemp).period(minar::milliseconds(1000));

	        setState(STATE_NOT_CONNECTED);
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
