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
 * Component   : Bluetooth utility
 * File        : cb_bt_utils.h
 *
 * Description : 
 *-------------------------------------------------------------------------*/

#ifndef _CB_BT_UTILS_H_
#define _CB_BT_UTILS_H_

#include "cb_comdefs.h"
#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * FUNCTIONS
 *=========================================================================
 */

/**
 * Compare two Bluetooth addresses
 *
 * @param addr1   Pointer to first address to compare
 * @param addr2   Pointer to second address to compare
 * @returns TRUE if equal otherwise FALSE
 */
extern cb_boolean cbBT_UTILS_cmpBdAddr(
    TBdAddr* addr1, 
    TBdAddr* addr2);

/**
 * Check if address in invalid i.e. {{0,0,0,0,0,0},BT_PUBLIC_ADDRESS}
 *
 * @param addr    Pointer to address to check
 * @returns TRUE if invalid otherwise FALSE
 */
extern cb_boolean cbBT_UTILS_isInvalidBdAddr(
    TBdAddr* addr);

/**
 * Set invalid address i.e. {{0,0,0,0,0,0},BT_PUBLIC_ADDRESS}
 *
 * @param addr    Pointer where to put the address
 */
extern void cbBT_UTILS_setInvalidBdAddr(
    TBdAddr* addr);

/**
 * Get invalid address
 *
 * @returns Pointer to the invalid address
 */
extern const TBdAddr* cbBT_UTILS_getInvalidBdAddr(void);

/**
 * Copy Bluetooth address
 *
 * @param dest   Pointer to destination address
 * @param src    Pointer to source address
 */
extern void cbBT_UTILS_cpyBdAddr(
    TBdAddr* dest, 
    TBdAddr* src);

#ifdef __cplusplus
}
#endif

#endif /* _CB_BT_UTILS_H_ */