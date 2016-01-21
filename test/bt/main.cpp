/*
* PackageLicenseDeclared: Apache-2.0
* Copyright (c) 2015 u-blox
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
#include "mbed-drivers/mbed.h"
#include "minar/minar.h"

// Force float printf support
asm(".global _printf_float");

extern "C" {
#include "btc_app.h"
#include "ublox-odin-w2-drivers-binary\bt_types.h"
#include "ublox-odin-w2-drivers-binary\cb_bt_utils.h"
#include "ublox-odin-w2-drivers-binary\cb_bt_man.h"
#include "ublox-odin-w2-drivers-binary\cb_main.h"
#include "ublox-odin-w2-drivers-binary\cb_bt_serial.h"
}

#define BT_MAIN_TEST_DATA_SIZE (512)
#define bt_main_write_tag (666)
#define bt_main_write_delay_ms (10)
#define BT_MAIN_OK(i) (i == BTC_APP_OK ? TRUE : FALSE)
#define main_bt__MIN(x , y)   (((x) < (y)) ? (x) : (y))

typedef enum
{
    btMAIN_initializing = 0,
    btMAIN_Idle,
    btMAIN_Inquiring,
    btMAIN_InquiryCompleted,
    btMAIN_Bonding,
    btMAIN_bondingCompleted,
    btMAIN_ConnectingSPP,
    btMAIN_Disconnecting,
    btMAIN_SPPConnected,
    btMAIN_StreamOpened
}btMAIN_State;

static void buttonPressedAction(void);
static void InquiryCnfEvent(btcAPPe status, uint16_t nbrOfDevices);
static void BondCnfEvent(btcAPPe status);
static void ConnectCnfEvent(btcAPPe status, cb_int16 handle, TBdAddr* bdAddr);
static void DisonnectEvent(cb_int16 handle, TBdAddr* bdAddr);
static void DataAvailableEvt(int16_t handle);
static void StreamWriteCnf(cbBCM_Handle handle, cb_int32 status, cb_uint32 nBytes, cb_int32 tag);
static void controllerStartupComplete(void);
static void testBonding(void);
static void sendDataToDevices(void);
static void disconnectDevices(void);

//static  TBdAddr XperiaZ3 = { { 0x40, 0xb8, 0x37, 0x5c, 0x79, 0x5f }, BT_PUBLIC_ADDRESS };
static  TBdAddr com08 = { { 0x78, 0xA5, 0x04, 0x2F, 0x4A, 0xDE }, BT_PUBLIC_ADDRESS };
static  TBdAddr com09 = { { 0x78, 0xA5, 0x04, 0x2F, 0x4A, 0xED }, BT_PUBLIC_ADDRESS };
static  TBdAddr com21 = { { 0x78, 0xA5, 0x04, 0x2F, 0x02, 0x60 }, BT_PUBLIC_ADDRESS };
static  TBdAddr com22 = { { 0x78, 0xA5, 0x04, 0x2F, 0x03, 0x06 }, BT_PUBLIC_ADDRESS };
static  TBdAddr com28 = { { 0x00, 0x12, 0xf3, 0x27, 0x46, 0xf6 }, BT_PUBLIC_ADDRESS };

static  TBdAddr remoteDevices[btcAPP_MAX_CONNECTIONS] = { com08, com09, com22, com21, com28 };

typedef struct
{
    btMAIN_State    app_state;
    uint16_t        nbrOfFoundDevices;
    uint8_t         nbrOfConnectedDev;
    uint8_t         nbrOfBondedDev;
    uint8_t         nextDeviceToBond;
    uint8_t         nextDeviceToConnect;
    int8_t          connDevHandle[btcAPP_MAX_CONNECTIONS];
    int16_t         nextDevHandle;
    bool            discDevices;
    void*           pTestData;
} btAPP_CLASS;

static const btcAPP_EventCallBack appEventCallback =
{
    InquiryCnfEvent,
    BondCnfEvent,
    ConnectCnfEvent,
    DisonnectEvent,
    DataAvailableEvt,
    StreamWriteCnf
};

//DigitalIn button_SW1(SW1);
InterruptIn button_SW1(SW1);
static bool buttonPressed = false;
static btAPP_CLASS thisApp;

//===============================================================>
/*Minar callback handlers*/
//minar::callback_handle_t _post_handle(NULL);

static void addHandle(int16_t handle)
{
    int i;
    for (i = 0; i < btcAPP_MAX_CONNECTIONS; i++)
    {
        if (thisApp.connDevHandle[i] == -1)
        {
            thisApp.connDevHandle[i] = handle;
            break;
        }
    }
}

static void removeHandle(int16_t handle)
{
    int i;
    for (i = 0; i < btcAPP_MAX_CONNECTIONS; i++)
    {
        if (thisApp.connDevHandle[i] == handle)
        {
            thisApp.connDevHandle[i] = -1;
            break;
        }
    }
}

static int16_t nextHandleToWrite(void)
{
    int8_t i;
    bool found = false;
    for (i = 0; i < btcAPP_MAX_CONNECTIONS; i++)
    {
        if (thisApp.connDevHandle[i] == thisApp.nextDevHandle)
        {
            i++;
            break;
        }
    }
    while (!found)
    {
        if (i >= btcAPP_MAX_CONNECTIONS)
        {
            i = 0;
        }
        else if (thisApp.connDevHandle[i] != -1)
        {
            found = true;
        }
        else
        {
            i++;
        }
    }
    return thisApp.connDevHandle[i];
}

static void InquiryCnfEvent(btcAPPe status, uint16_t nbrOfDevices)
{
    if (BT_MAIN_OK(status))
    {
        thisApp.app_state = btMAIN_InquiryCompleted;
        thisApp.nbrOfFoundDevices = nbrOfDevices;
    }
    else
    {
        thisApp.app_state = btMAIN_Idle;
        thisApp.nbrOfFoundDevices = 0;
    }
}

static void BondCnfEvent(btcAPPe status)
{
    if (BT_MAIN_OK(status))
    {
        thisApp.nbrOfBondedDev++;
    }
    else
    {
        printf("Bonding failed for device nr:%d\n", thisApp.nextDeviceToBond);
    }
    if (thisApp.nextDeviceToBond == btcAPP_MAX_CONNECTIONS)
    {
        thisApp.app_state = btMAIN_bondingCompleted;
        printf("Bonding Completed. nbrOfBondedDev=%d\n", thisApp.nbrOfBondedDev);
    }
    else
    {
        minar::Scheduler::postCallback(&testBonding).delay(minar::milliseconds(1000));
    }
}

static void ConnectCnfEvent(btcAPPe status, cb_int16 handle, TBdAddr* bdAddr)
{
    (void)bdAddr;
    printf("ConnectCnfEvent --> status=%ld handle=%d\n", status, handle);
    if (BT_MAIN_OK(status))
    {
        addHandle(handle);
        thisApp.app_state = btMAIN_SPPConnected;
        thisApp.nbrOfConnectedDev++;
    }
    else
    {
        thisApp.app_state = btMAIN_bondingCompleted;
    }
}

static void DisonnectEvent(cb_int16 handle, TBdAddr* bdAddr)
{
    (void)bdAddr;
    printf("Disconnected handle:%d\n", handle);

    thisApp.nbrOfConnectedDev--;
    removeHandle(handle);
    if (thisApp.nbrOfConnectedDev == 0)
    {
        thisApp.app_state = btMAIN_bondingCompleted;
        thisApp.nextDeviceToConnect = 0;
        thisApp.discDevices = false;
    }
    else if (thisApp.discDevices)
    {
        minar::Scheduler::postCallback(&disconnectDevices).delay(minar::milliseconds(1500));
    }
}

static void DataAvailableEvt(int16_t handle)
{
    btcAPPe result = BTC_APP_ERROR;
    uint8_t *pData;
    uint32_t len, size;

    do
    {
        result = cbBSE_getReadBuf(handle, &pData, &len);
        if (result == BTC_APP_OK)
        {
            size = main_bt__MIN(BT_MAIN_TEST_DATA_SIZE, len);
            memcpy((void*)thisApp.pTestData, (void*)pData, size);
            cbBSE_readBufConsumed(handle, len);
            result = btcAPPwrite(handle, pData, size, bt_main_write_tag);
            if (!BT_MAIN_OK(result))
            {
                printf("Failed to send to handle:%d\n", handle);
            }
        }
    } while (result == BTC_APP_OK);
}

static void StreamWriteCnf(cbBCM_Handle handle, cb_int32 status, cb_uint32 nBytes, cb_int32 tag)
{
    (void)tag;
    (void)nBytes;

    printf("StreamWriteCnf status:%ld handle=%ld\n", status, handle);
    if (!thisApp.discDevices)
    {
        minar::Scheduler::postCallback(&sendDataToDevices).delay(minar::milliseconds(bt_main_write_delay_ms));
    }
}

static void controllerStartupComplete(void)
{
    btcAPPe result;
    
    cbMAIN_initWlan();

    btcAPPinit();

    result = btcAPPregisterCallBacks((btcAPP_EventCallBack*) &appEventCallback);

    if (BT_MAIN_OK(result))
    {
        thisApp.app_state = btMAIN_Idle;
    }
}

static void testBonding(void)
{
    btcAPPe result = BTC_APP_ERROR;
    
    if (thisApp.nextDeviceToBond < btcAPP_MAX_CONNECTIONS)
    {
        do
        {
            result = btcAPPreqBond(&remoteDevices[thisApp.nextDeviceToBond]);
            thisApp.nextDeviceToBond++;
            
        } while (!BT_MAIN_OK(result) && (thisApp.nextDeviceToBond != btcAPP_MAX_CONNECTIONS));
    }
    
    if (thisApp.nextDeviceToBond == btcAPP_MAX_CONNECTIONS)
    {
        thisApp.app_state = btMAIN_bondingCompleted;
    }
}

static void sendDataToDevices(void)
{
    int32_t result = BTC_APP_ERROR;

    if (!thisApp.discDevices)
    {
        result = btcAPPwrite(thisApp.nextDevHandle, (uint8_t*)thisApp.pTestData, BT_MAIN_TEST_DATA_SIZE, thisApp.nextDevHandle);

        if (!BT_MAIN_OK(result))
        {
            printf("Failed to send to handle:%d\n", thisApp.nextDevHandle);
            minar::Scheduler::postCallback(&sendDataToDevices).delay(minar::milliseconds(bt_main_write_delay_ms));
        }
        else
        {
            printf("Sent %d bytes to handle:%d\n", BT_MAIN_TEST_DATA_SIZE, thisApp.nextDevHandle);
        }
        thisApp.nextDevHandle = nextHandleToWrite();
    }
}

static void disconnectDevices(void)
{
    int32_t result = BTC_APP_ERROR;
    int i;

    for (i = 0; i < btcAPP_MAX_CONNECTIONS; i++)
    {
        if (thisApp.connDevHandle[i] != -1)
        {
            result = btcAPPdisconnect(thisApp.connDevHandle[i]);
            printf("btcAPPdisconnect for handle %d returned %ld\n", thisApp.connDevHandle[i], result);
            break;
        }
    }
    if (i == btcAPP_MAX_CONNECTIONS)
    {
        thisApp.discDevices = false;
    }
}

static void buttonPressedAction(void)
{
    int32_t result = BTC_APP_ERROR;
    int i;

    printf("buttonPressedAction state=%d\n", thisApp.app_state);
    
    switch (thisApp.app_state)
    {
        case btMAIN_Idle:
            result = btcAPPinquiry();
            if (BT_MAIN_OK(result))
            {
                thisApp.app_state = btMAIN_Inquiring;
            }
            break;

        case btMAIN_InquiryCompleted:
            if (thisApp.nextDeviceToBond == btcAPP_MAX_CONNECTIONS)
                thisApp.nextDeviceToBond = 0;
        
            testBonding();
            thisApp.app_state = btMAIN_Bonding;
            break;

        case btMAIN_bondingCompleted:
        case btMAIN_SPPConnected:
            if (thisApp.nextDeviceToConnect < btcAPP_MAX_CONNECTIONS)
            {
                result = btcAPPrequestConnectSPP(&remoteDevices[thisApp.nextDeviceToConnect]);
                if (BT_MAIN_OK(result))
                {
                    thisApp.app_state = btMAIN_ConnectingSPP;
                }
                thisApp.nextDeviceToConnect++;
                printf("SPP connection result %ld nr of connected devices:%d\n", result, thisApp.nbrOfConnectedDev);
                printf("next device to connect:%d\n", thisApp.nextDeviceToConnect);
            }
            else if (thisApp.nbrOfConnectedDev > 0 && !thisApp.discDevices)
            {
                //start sending data
                for (i = 0; i < btcAPP_MAX_CONNECTIONS; i++)
                {
                    if (thisApp.connDevHandle[i] != -1)
                    {
                        thisApp.nextDevHandle = thisApp.connDevHandle[i];
                        break;
                    }
                }
                sendDataToDevices();
                thisApp.app_state = btMAIN_StreamOpened;
            }
            break;
        case btMAIN_StreamOpened:
                thisApp.discDevices = true;
                thisApp.app_state = btMAIN_Disconnecting;
                disconnectDevices();
            break;

        default:
            break;
    }
}

void button_SW1_PressedInt()
{
    if (!buttonPressed)
    {
        buttonPressed = true;
        minar::Scheduler::postCallback(&buttonPressedAction).delay(minar::milliseconds(100));
    }
    else
    {
        // ignore, two falling edges occur for each press and release
        printf("Button SW1 released \n");
        buttonPressed = false;
    }
}

void app_start(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    int i;
    cbBM_InitParams initParams;

    button_SW1.fall(&button_SW1_PressedInt);
    thisApp.app_state = btMAIN_initializing;
    thisApp.pTestData = malloc(BT_MAIN_TEST_DATA_SIZE);
    thisApp.nbrOfConnectedDev = 0;
    thisApp.nbrOfBondedDev = 0;
    thisApp.nextDeviceToBond = 0;
    thisApp.nextDeviceToConnect = 0;
    thisApp.nextDevHandle = -1;
    thisApp.discDevices = false;
    memset(thisApp.pTestData, 's', BT_MAIN_TEST_DATA_SIZE);

    for (i = 0; i < btcAPP_MAX_CONNECTIONS; i++)
    {
        thisApp.connDevHandle[i] = -1;
    }

    //cbMAIN_init_bt(controllerStartupComplete); // TODO move, this will change CPU speed. Must be called before any SPI/I2C/UART initialization    

    cbBT_UTILS_setInvalidBdAddr(&initParams.address);
    initParams.leRole = cbBM_LE_ROLE_CENTRAL;
    initParams.maxOutputPower = cbBM_MAX_OUTPUT_POWER;
    initParams.nvdsStartIdLinkKeysClassic = 1000 + 2300;
    initParams.maxLinkKeysClassic = 25;
    initParams.nvdsStartIdLinkKeysLe = 1000 + 2500;
    initParams.maxLinkKeysLe = 25;
    cbMAIN_initBt(&initParams, controllerStartupComplete);
}
