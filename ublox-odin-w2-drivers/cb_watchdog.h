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
 * Component:   Watchdog
 * File:        cb_watchdog.h
 *
 * Description: Functionality for watchdog and reset.
 *-------------------------------------------------------------------------*/
#ifndef _CB_WATCHDOG_H_
#define _CB_WATCHDOG_H_

#include "cb_comdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/


/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/**
* Put watchdog in a defined state.
*/
void cbWD_init(void);

/**
* Resets the CPU.
*/
void cbWD_systemReset(void);

/**
* Enables watchdog. Watchdog needs to be polled using cbWD_poll() with
* shorter intervals then specified by timeInMilliseconds.
*
* @param timeInMilliseconds Watchdog timeout in milliseconds.
*/
void cbWD_enable(cb_uint32 timeInMilliseconds);

/**
* Poll the watchdog timer. This must be done with shorter intervalls
* than the time in cbWD_enable().
*/
void cbWD_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* _CB_WATCHDOG_H_ */
