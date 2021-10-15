/**
 *---------------------------------------------------------------------------
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
 * Component   : Bluetooth Serial
 * File        : cb_bt_serial_le.h
 *
 * Description : Data management for Serial Port Service.
 *
 *-------------------------------------------------------------------------*/

/**
 * @file cb_bt_serial_le.h
 * @brief Data management for Serial Port Service.
 */

#ifndef _CB_BT_SERIAL_LE_H_
#define _CB_BT_SERIAL_LE_H_

#include "cb_comdefs.h"
#include "bt_types.h"
#include "cb_bt_conn_man.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
* DEFINES
*=========================================================================*/
#define cbBSL_OK                       0
#define cbBSL_ERROR                   -1
#define cbBSL_NO_DATA                 -2

/*===========================================================================
* TYPES
*=========================================================================*/

typedef void (*cbBSL_DataAvailEvt)(
    cbBCM_Handle handle);

typedef void (*cbBSL_WriteCnf)(
    cbBCM_Handle handle,
    cb_int32 status,
    cb_uint32 nBytes,
    cb_int32 tag);

typedef struct
{
    cbBSL_DataAvailEvt        pfDataEvt;
    cbBSL_WriteCnf            pfWriteCnf;
}cbBSL_Callback;

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/
/**
 * Initialization of Bluetooth serial manager. Called during stack
 * initialization. Shall not be called by application.
 * 
 * @return None
 */
extern void cbBSL_init(void);

/**
 * Open a data channel.
 * 
 * @param   handle      Connection handle
 * @param   pCallback   Callback for data events.
 * @return  If the operation is successful cbBSL_OK is returned.
 */
extern cb_int32 cbBSL_open(
    cbBCM_Handle handle,
    cbBSL_Callback *pCallback);

/**
 * Send data to remote device. A data confirmation event is generated when
 * the data has been sent and a cbBSL_write call can be done.
 * 
 * Detailed description optionally verbose.
 * @param   handle  Connection handle
 * @param   pBuf    Data pointer
 * @param   nBytes  nBytes Size of data to be sent.
 * @param   tag     Tag passed as argument in corresponding data confirmation callback.
 * @return  If the operation is successful cbBSL_OK is returned.
 */
extern cb_int32 cbBSL_write(
    cbBCM_Handle handle,
    cb_uint8 *pBuf,
    cb_uint32 nBytes,
    cb_int32 tag);

/**
 * Get received data.
 * 
 * @param handle Connection handle
 * @param   ppBuf    Pointer to data buffer
 * @param   pLength  Pointer to buffer length variable.
 * @return  cbBSL_OK is returned if data is available. If no data is available
 *          then cbBSL_NO_DATA is returned.
 */
extern cb_int32 cbBSL_getReadBuf(
    cbBCM_Handle handle,
    cb_uint8 **ppBuf,
    cb_uint32 *pLength);

/**
 * Notify that received data has been handled and underlying buffers 
 * can be freed.
 *
 * @param handle Connection handle
 * @param nBytes Number of bytes consumed.
 * @return If the operation is successful cbBSL_OK is returned.
 */
extern cb_int32 cbBSL_readBufConsumed(
    cbBCM_Handle handle,
    cb_uint32 nBytes);

/**
 * Read max frame size for a data channel.
 * 
 * @param   handle      Connection handle
 * @param   pFrameSize  Max frame size for connection.
 * @return  If the operation is successful cbBSE_OK is returned.
 */
extern cb_int32 cbBSL_frameSize(cbBCM_Handle handle, cb_uint32 *pFrameSize);

/**
 * Bluetooth serial message handling. Shall not be called by application.
 * 
 * @param msgId Message id
 * @param pData Pointer to message data
 * @return None
 */
extern void cbBSL_handleMsg(cb_uint32 msgId, void* pData);

#ifdef __cplusplus
}
#endif

#endif
