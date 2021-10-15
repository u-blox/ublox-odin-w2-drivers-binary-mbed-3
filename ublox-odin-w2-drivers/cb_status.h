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
 * Component   : RTSL
 * File        : cb_status.h
 *
 * Description : Common RTSL status codes 
 *-------------------------------------------------------------------------*/
#ifndef _CB_STATUS_H_
#define _CB_STATUS_H_

/*===========================================================================
 * DEFINES
 *=========================================================================*/

#define OK(status) (status == cbSTATUS_OK)
#define BUSY(status) (status == cbSTATUS_BUSY)
#define ERR(status) (status == cbSTATUS_ERROR)

/*===========================================================================
 * TYPES
 *=========================================================================*/

 typedef enum
 {
    cbSTATUS_OK,
    cbSTATUS_ERROR,
    cbSTATUS_BUSY,
    cbSTATUS_RECEIVE_DATA_MODE,
    cbSTATUS_TIMEOUT
 
 } cbRTSL_Status;

#endif /* _CB_STATUS_H_ */

