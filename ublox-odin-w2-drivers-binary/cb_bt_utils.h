/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox, Sweden.
 * Any reproduction without written permission is prohibited by law.
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

extern cb_boolean cbBT_UTILS_cmpBdAddr(
    TBdAddr* addr1, 
    TBdAddr* addr2);

extern cb_boolean cbBT_UTILS_isInvalidBdAddr(
    TBdAddr* addr);

extern void cbBT_UTILS_setInvalidBdAddr(
    TBdAddr* addr);

extern const TBdAddr* cbBT_UTILS_getInvalidBdAddr(void);

extern void cbBT_UTILS_cpyBdAddr(
    TBdAddr* dest, 
    TBdAddr* src);

#ifdef __cplusplus
}
#endif

#endif /* _CB_BT_UTILS_H_ */