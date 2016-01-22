/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : WLAN
 * File        : cb_wlan_os.c
 *
 * Description : WLAN component OS abstraction / helper class.
 *-------------------------------------------------------------------------*/

/**
 * @file cb_hfsm.h WLAN component OS abstraction / helper class.
 * @ingroup wlan
 */

#ifndef _CB_WLAN_OS_H_
#define _CB_WLAN_OS_H_

#include "cb_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/
typedef struct cbWLAN_OS_REPrivHandle_s cbWLAN_OS_REPrivHandle;

typedef cbRTSL_Status (*cbWLAN_OS_SynchronizedCallback)(cbWLAN_OS_REPrivHandle *, void *);
typedef void (*cbWLAN_OS_TickCallback)(cbWLAN_OS_REPrivHandle *, cb_uint32);

/*===========================================================================
 * EXTERNAL API
 *=========================================================================*/

/**
 * Registers WLAN component with the WLAN_OS execution environment (EE).
 *
 * @note The EE is expected to call the tick function every ~100ms in order 
 * to feed the internal timers in WLAN component. Before calling the tick 
 * function, the EE callback lock must be acquired. @sa cbWLAN_OS_synchronize
 *
 * @param hWlan Pointer to struct containing necessary WLAN references.
 * @param tick Pointer to timer tick callback.
 * @return Should return @ref cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR. 
 */
extern cbRTSL_Status cbWLAN_OS_register(cbWLAN_OS_REPrivHandle *hWlan, cbWLAN_OS_TickCallback tick);

/**
 * Unregisters WLAN component with the WLAN_OS execution environment.
 *
 * @param hWlan WLAN component handle registered in @ref cbWLAN_OS_register
 * @return Should return @ref cbSTATUS_OK if successful, otherwise cbSTATUS_ERROR. 
 */
extern cbRTSL_Status cbWLAN_OS_unregister(cbWLAN_OS_REPrivHandle *hWlan);

/**
 * "Tunnel" a function call through the WLAN_OS execution environment (EE).
 * 
 * @note The EE may not queue the function call, it must be done immediately.
 * Because of the design of the WLAN component, a semaphore or similar
 * locking mechanism must be used. This lock must be acquired before calling
 * the callback function.
 *
 * @param hWlan WLAN component handle registered in @ref cbWLAN_OS_register
 * @param func Callback function pointer.
 * @param data Anonymous data pointer that should be passed back to @ref func.
 * @return Return value of func should be passed back as return value.
 */
extern cbRTSL_Status cbWLAN_OS_synchronize(cbWLAN_OS_REPrivHandle *hWlan, cbWLAN_OS_SynchronizedCallback func, void *data);

/**
 * Check if the EE is currently active or inactive.
 *
 * Used in cases where the WLAN component need to check whether it is an
 * initial call or a reentrant call (i.e. originating from a callback).
 *
 * @param hWlan WLAN component handle registered in @ref cbWLAN_OS_register
 * @return TRUE if synchronized, FALSE otherwise.
 */
extern cb_boolean cbWLAN_OS_isSynchronized(cbWLAN_OS_REPrivHandle *hWlan);

/**
 * Get millisecond resolution tick from WLAN_OS execution environment (EE).
 * Incrementing time tick from EE. The cb_uint32 is allowed to wrap.
 *
 * @param hWlan WLAN component handle registered in @ref cbWLAN_OS_register
 * @return Millisecond resolution system tick.
 */
extern cb_uint32 cbWLAN_OS_getTicks(cbWLAN_OS_REPrivHandle *hWlan);

/**
 * Acquire a second level lock.
 * Can be implemented as a spinlock or a mutex. If a preemptive system is used 
 * and @ref cbWLAN_OS_synchronize is implemented to handle it, this may even
 * be left empty. 
 * This function shall never be called recursively, i.e. only one lock
 * instance need to be available.
 *
 * @param hWlan WLAN component handle registered in @ref cbWLAN_OS_register
 */
extern void cbWLAN_OS_SL_acquire(cbWLAN_OS_REPrivHandle *hWlan);

/**
 * Release a second level lock.
 * @sa cbWLAN_OS_SL_acquire
 *
 * @param hWlan WLAN component handle registered in @ref cbWLAN_OS_register
 */
extern void cbWLAN_OS_SL_release(cbWLAN_OS_REPrivHandle *hWlan);

void cbWLAN_OS_ISRHandler(void* hCallback);

extern void cbWLAN_OS_interrupt();

#ifdef __cplusplus
}
#endif

#endif /* _CB_WLAN_OS_H_ */
