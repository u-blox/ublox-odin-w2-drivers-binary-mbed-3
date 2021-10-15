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
 * Component   : OTP
 * File        : cb_otp.h
 *
 * Description : Support for One Time Programmable memory intended for
 *               storing production parameters such as mac addresses, trim
 *               values and product configuration. Writing to OTP memory shall 
 *               only be done in a production environment.
 *-------------------------------------------------------------------------*/

/**
 * @file cb_otp.h 
 * @ingroup platform
 */

#ifndef _CB_OTP_H_
#define _CB_OTP_H_

#include "cb_comdefs.h"
#include "cb_status.h"
#include "cb_rtsl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    cbOTP_MAC_BLUETOOTH = 1,
    cbOTP_MAC_WLAN,
    cbOTP_MAC_ETHERNET,
    cbOTP_MAC_FEATURE_INFO,
    cbOTP_MAC_DEBUG_UNIT,
    cbOTP_SERIAL_NUMBER,
    cbOTP_TYPE_CODE,
    cbOTP_SIGNATURE,
    cbOTP_MAC_WLAN_AP,
    cbOTP_UUID,
    cbOTP_RF_FREQ_CALIBRATION,
    cbOTP_RESERVED_UNUSED = 255
} cbOTP_Id;


/**
 * Read a OTP parameter
 * @param id    The id of the parameter to write
 * @param len   The length of the parameter to write
 * @param buf   Pointer to data to be written
 * @returns     The read length of the id is returned. If the read fails 0 is returned
 */
cb_uint32 cbOTP_read(cbOTP_Id id, cb_uint32 len, cb_uint8 *buf);

#ifdef __cplusplus
}
#endif

#endif
