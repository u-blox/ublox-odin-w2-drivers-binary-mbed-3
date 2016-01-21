/*---------------------------------------------------------------------------
 * Copyright (c) 2006 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component: RTSL
 * File     : cb_timer.h
 *
 * Description: Contains functionality for software timers.
 *-------------------------------------------------------------------------*/
#ifndef _CB_TIMER_H_
#define _CB_TIMER_H_

#include "cb_comdefs.h"


/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define cbTIMER_INVALID_ID      ((cb_int32)-1)

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef cb_int32 cbTIMER_Id;

typedef void (*cbTIMER_Notify)(
    cbTIMER_Id  id,
    cb_int32    tag1,
    cb_int32    tag2);


/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/*---------------------------------------------------------------------------
 * Initialises the timer module. Typically called from start handler. 
 * Must be called after cbOS_init.
 *-------------------------------------------------------------------------*/
void cbTIMER_init(void);


/*---------------------------------------------------------------------------
 * Starts a periodic timer. Resolution specified by cbTIMER_RESOLUTION_MS.
 * - period: Time in milliseconds.
 * - expireCallbFcn: Callback function that is called when timer expires.
 * - tag1: User defined tag that is used in callback.
 * - tag2: User defined tag that is used in callback.
 * The function returns the timer identifier.
 *-------------------------------------------------------------------------*/
cbTIMER_Id cbTIMER_every(
    cb_uint32              period,
    cbTIMER_Notify         expireCallbFcn,
    cb_int32               tag1,
    cb_int32               tag2);


/*---------------------------------------------------------------------------
 * Sets a timer to expire after delay milliseconds with a resolution specified
 * by cbTIMER_RESOLUTION_MS.
 * - delay: Time in milliseconds.
 * - expireCallbFcn: Callback function that is called when timer expires.
 * - tag1: User defined tag that is used in callback.
 * - tag2: User defined tag that is used in callback.
 * The function returns the timer identifier.
 *-------------------------------------------------------------------------*/
cbTIMER_Id cbTIMER_after(
    cb_uint32              delay,
    cbTIMER_Notify         expireCallbFcn,
    cb_int32               tag1,
    cb_int32               tag2);


/*---------------------------------------------------------------------------
 * Cancels a previously started timer. Note, that the timer might already
 * be expired and the expire callback function might just have been called.
 * - id: Timer identifier returned in the cbTIMER_after call.
 *-------------------------------------------------------------------------*/
void cbTIMER_cancel(cbTIMER_Id id);

#endif /* _CB_TIMER_H_ */


