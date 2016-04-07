#ifndef _CB_MAIN_H_
#define _CB_MAIN_H_

/*---------------------------------------------------------------------------
 * Copyright (c) 2015 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component: Main for WiFi-driver and BT stack
 * File     : cb_main.h
 *
 * Description : 
 *-------------------------------------------------------------------------*/

#include "bt_types.h"
#include "cb_bt_man.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

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

extern void cbMAIN_initBt(cbBM_InitParams *pInitParameters, cbMAIN_initBtComplete callback);
extern cb_int32 cbMAIN_initWlan(void);
extern void cbMAIN_initEth(void);
extern void cbMAIN_registerErrorHandler(cbMAIN_ErrorHandler errHandler);

#ifdef __cplusplus
}
#endif

#endif /*_CB_MAIN_H_*/
