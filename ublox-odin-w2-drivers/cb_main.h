/*---------------------------------------------------------------------------
 * Copyright (C) u-blox
 *
 * u-blox reserves all rights in this deliverable (documentation, software,
 * etc., hereafter "Deliverable").
 *
 * This file is the sole property of u-blox. Its reproduction, modification,
 * re-distribution, sub-licensing or any other use is subject to u-blox's
 * General Terms and Conditions of Sales or to specific terms applicable to
 * the file as indicated by u-blox.
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
#ifdef CB_SUPPORT_ETH
#include "cb_ethernet.h"
#endif

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
* A default configuration is used.
* 
* @param targetId Port specific TARGET identifier.
* @param params Start parameters passed to WLAN driver instance.
* @return cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR.
*/
extern cb_int32 cbMAIN_startWlan(cb_int32 targetId, cbWLAN_StartParameters *params);

/**
* Start WLAN component with no default config. This should be used if there is
* a need for a non-default configuration on startup e.g. force world mode.
* Prior to calling this function the user must set a configuration e.g. with cbTARGET_configuration_create
* Create WLAN driver instance, bind it to targetId and start the driver.
*
* @param targetId Port specific TARGET identifier.
* @param params Start parameters passed to WLAN driver instance.
* @return cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR.
*/
extern cb_int32 cbMAIN_startWlanNoConfig(cb_int32 targetId, cbWLAN_StartParameters *params);

/**
* Reset the driver and Wi-Fi chip. This can be used if the application receives a cbWLAN_STATUS_ERROR event.
* The recovery procedure is as follows:
* - Application receives cbWLAN_STATUS_ERROR
* - Application calls cbWLAN_stop
* - Application calls cbMAIN_resetWlan after the cbWLAN_STATUS_STOPPED is received
* - Application calls cbMAIN_startWlan
* - If we're station then reconnect if needed
* If the application acts as station it will also receive a cbWLAN_STATUS_DISCONNECTED
* before the cbWLAN_STATUS_ERROR if it was previously connected.
* 
* @param targetId Port specific TARGET identifier.
* @return new targetId.
*/
extern cb_int32 cbMAIN_resetWlan(cb_int32 targetId);

#ifdef CB_SUPPORT_ETH
/**
* Initialize Ethernet.
*
* @param pConfig    Pointer to config to use. If NULL is passed a default config 
                    is used with the preprogrammed address.
* @return cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR.
*/
extern cb_int32 cbMAIN_initEth(cbETH_Config* pConfig);

/**
* Get ethernet default config.
*
* @return   Pointer to default config 
*/
extern const cbETH_Config* cbMAIN_getDefaultEthConfig(void);

#endif /* CB_SUPPORT_ETH */
/**
* Register error handler function.
*
* @param errHandler Function to be invoked in case of error.
* @return void
*/
extern void cbMAIN_registerErrorHandler(cbMAIN_ErrorHandler errHandler);

/**
* Changes the log level for debug prints. Can be called after init.
*
* @param level Log level: 0  Logging is turned off
*                         1  Only errors are logged
*                         2  All functions/callbacks except data are logged
*                         3  All functions/callbacks including data are logged
*                         4  Everything. To be used with care, flood warning
*
* @return void
*/
extern void cbMAIN_setLogLevel(cb_uint8 level);

#ifdef __cplusplus
}
#endif

#endif /*_CB_MAIN_H_*/
