/*---------------------------------------------------------------------------
 * Copyright (c) 2014 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
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

#define cbOTP_MAX_SIZE          (30)

typedef enum
{
    cbOTP_MAC_BLUETOOTH = 1,
    cbOTP_MAC_WLAN,
    cbOTP_MAC_ETHERNET,
    cbOTP_MAC_FEATURE_INFO,
    cbOTP_MAC_DEBUG_UNIT,
    cbOTP_SERIAL_NUMBER,
    cbOTP_TYPE_CODE,
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

#endif
