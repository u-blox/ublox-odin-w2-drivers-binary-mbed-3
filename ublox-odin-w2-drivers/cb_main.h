/*---------------------------------------------------------------------------
 * Copyright (c) 2015 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component: Main for WiFi-driver and BT stack
 * File     : cb_main.h
 *
 * Description : 
 *-------------------------------------------------------------------------*/

#ifndef _CB_MAIN_H_
#define _CB_MAIN_H_

#include "bt_types.h"
#include "cb_bt_man.h"
#include "cb_wlan.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/
typedef struct
{
    TBdAddr         address;                    /** Bluetooth address that shall be assigned to controller. Pass invalidBdAddress to use controller default address*/
    cbBM_LeRole     leRole;                     /** Bluetooth low energy role */
    cb_int8         maxOutputPower;             /** Maximum output power. */
    cb_uint32       maxLinkKeysClassic;         /** Max number of CLASSIC link keys */
    cb_uint32       maxLinkKeysLe;              /** Max number of link keys BLE*/
} cbMAIN_BtInitParams;

typedef void(*cbMAIN_ErrorHandler)(
    cb_int32            errorCode,
    const cb_char*      filename,
    cb_uint32           line);

/*---------------------------------------------------------------------------
* Callback to indicate that initialization of BT stack is completed.
*-------------------------------------------------------------------------*/
typedef void(*cbMAIN_initBtComplete)(void);

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/**
* Initialize OS, timers, GPIO's, heap and OTP.
*
* @return void
*/
extern void cbMAIN_initOS(void);

/**
* Start Bluetooth HW.
*
* @param pInitParameters Initial configuration parameters. These parameters can
*                        not be changed once Bluetooth has been started.
* @param callback Will be invoked when initialisation is done.
* @return void
*/
extern void cbMAIN_initBt(cbMAIN_BtInitParams *pInitParameters, cbMAIN_initBtComplete callback);

/**
* Initialize WLAN component.
*/
extern cb_int32 cbMAIN_initWlan(void);

/**
* Start WLAN component.
* Create WLAN driver instance, bind it to targetId and start the driver.
*
* @param targetId Port specific TARGET identifier.
* @param params Start parameters passed to WLAN driver instance.
* @return cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR.
*/
extern cb_int32 cbMAIN_startWlan(cb_int32 targetId, cbWLAN_StartParameters *params);

/**
* Initialize Ethernet.
*
* @param pMacAddr Pointer to address to use. If NULL is passed the preprogrammed address will be used.
* @return Pointer to the MAC address.
*/
extern cb_uint8* cbMAIN_initEth(cb_uint8* pMacAddr);

/**
* Register error handler function.
*
* @param errHandler Function to be invoked in case of error.
* @return void
*/
extern void cbMAIN_registerErrorHandler(cbMAIN_ErrorHandler errHandler);

#ifdef __cplusplus
}
#endif

#endif /*_CB_MAIN_H_*/
