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
 * Component   : WLAN
 * File        : cb_wlan.h
 *
 * Description : Main WLAN component, ties together WM, SUPPLICANT and 
 *               TARGET to one streamlined API.
 *-------------------------------------------------------------------------*/

/**
 * @file cb_wlan.h The main WLAN component interface.
 * All functions declared extern needs to be provided by another/upper layer.
 * @ingroup wlan
 */

#ifndef _CB_CERT_UTILS_H_
#define _CB_CERT_UTILS_H_

#include "cb_types.h"
#include "cb_status.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define cbCERT_CRT_MAX_CHAIN_LENGTH        5ul

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef struct cbCERT_Stream_s cbCERT_Stream;
typedef cb_uint32 cbCERT_StreamPosition;

/**
 * Stream vtable interface used by WLAN supplicant to access SSL certificates
 * for WPA Enterprise authentication.
 *
 * @ingroup wlan
 */
struct cbCERT_Stream_s {
    cb_int32(*read)(const cbCERT_Stream *stream, void *buf, cb_uint32 count);  /**< Read function pointer, place count bytes in buf. */
    cb_int32(*write)(const cbCERT_Stream *stream, void *buf, cb_uint32 count);  /**< Read function pointer, place count bytes in buf. */
    void(*rewind)(const cbCERT_Stream *stream); /**< Rewind function pointer, rewind stream internal iterator to the beginning. Mandatory for all streams. */
    void(*setPosition)(const cbCERT_Stream *stream, cbCERT_StreamPosition position); /**< Set absolute position. */
    cbCERT_StreamPosition(*getPosition)(const cbCERT_Stream *stream); /**< Get current position. */
    cb_uint32(*getSize)(const cbCERT_Stream *stream);  /**< GetSize function pointer, return total size of stream contents. */
};

/*===========================================================================
 * CERT API
 *=========================================================================*/

cbRTSL_Status cbCERT_Util_parseDERCert(cbCERT_Stream const * const certificate, cbCERT_Stream const * const outputStream);
cbRTSL_Status cbCERT_Util_parseDERKey(cbCERT_Stream const * const key, cbCERT_Stream const * const outputStream);
cbRTSL_Status cbCERT_Util_parsePEMCert(cbCERT_Stream const * const certificate, cbCERT_Stream const * const outputStream);
cbRTSL_Status cbCERT_Util_parsePEMKey(cbCERT_Stream const * const certificate, cb_char const * const key, cb_uint32 keyLength, cbCERT_Stream const * const outputStream);

#ifdef __cplusplus
}
#endif

#endif /* _CB_CERT_UTILS_H_ */

