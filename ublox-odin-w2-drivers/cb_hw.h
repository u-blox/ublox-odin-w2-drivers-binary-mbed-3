/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component: CB HW
 * File     : cb_hw.h
 *
 * Description: Setup of hardware.
 *              TODO clean up this interface..
 *-------------------------------------------------------------------------*/
 
#ifndef _CB_HW_H_
#define _CB_HW_H_

#include "cb_comdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/
typedef enum {
    cbHW_PCB_VERSION_UNKNOWN,
    cbHW_PCB_VERSION_1,
    cbHW_PCB_VERSION_2,
    cbHW_PCB_VERSION_3,
    cbHW_PCB_VERSION_4,
    cbHW_PCB_VERSION_5
} cbHW_PCBVersion;

typedef enum {
    cbHW_RESET_REASON_UNKNOWN = 0,
    cbHW_RESET_REASON_FW_UPDATE,
    cbHW_RESET_REASON_PRODUCTION_MODE,
    cbHW_RESET_REASON_CRASH
}cbHW_ResetReason;

typedef enum {
    cbHW_FLOW_CONTROL_DISABLED = 0,
    cbHW_FLOW_CONTROL_ENABLED
} cbHW_FlowControl;

typedef enum {
    cbHW_IRQ_HIGH = 2U,
    cbHW_IRQ_MEDIUM = 3U,
    cbHW_IRQ_DEFAULT = 5U,
    cbHW_IRQ_LOW = 12U
}cbHW_PRIO_LVL;

typedef enum {
    cbHW_HASH_MD5
}cbHW_HashType;
/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef void (*cbHW_StopModeStatusEvt)(cb_boolean enable);
typedef void (*cbHW_SysTickCb)(void);

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

void cbHW_init(void);
void cbHW_registerStopModeStatusEvt(cbHW_StopModeStatusEvt evt);
void cbHW_disableIrq(void);
void cbHW_enableIrq(void);
void cbHW_disableAllIrq(void); // Should not be used unless extremely critical
void cbHW_enableAllIrq(void);
void cbHW_enterSleepMode(void);
void cbHW_enterStopMode(void);
void cbHW_setWakeupEvent(void);
void cbHW_resetWakeupEvent(void);

/**
 * Wait for specified amount of microseconds. May be interrupt dependent.
 * @note Granularity may vary between systems. Will be at least systick based.
 * The system may go to sleep during the delay.
 *
 * @param us Time to delay in microseconds.
 */
void cbHW_delay(cb_uint32 us);

/**
* Wait for specified amount of microseconds using a software loop.
* @note Granularity may vary between systems.
* The system will not go to sleep during the delay.
*
* @param us Time to delay in microseconds.
*/
void cbHW_softDelay(cb_uint32 us);
cb_boolean cbHW_sysFreqIsSupported(cb_uint32 sysFreq);
void cbHW_setSysFreq(cb_uint32 sysFreq);
cb_uint32 cbHW_getSysFreq(void);
void cbHW_writeBackupRegister(cb_uint32 registerId, cb_uint32 value);
cb_uint32 cbHW_readBackupRegister(cb_int32 registerId);
void cbHW_getHWId(cb_uint8 uid[cbHW_UNIQUE_ID_SIZE]);
cbHW_PCBVersion cbHW_getPCBVersion(void);

/**
* Register a system tick callback.
* The system tick will be generated once evert millisecond.
*
* @param cb Callback function for the system tick timer.
*/
void cbHW_registerSysTickISRCallback(cbHW_SysTickCb cb);

/**
* Get the current tick frequency for the @ref cbHW_getTicks tick counter.
* @note The frequency may be altered with different system clocks and power modes.
*
* @return The current tick frequency.
*/
cb_uint32 cbHW_getTickFrequency(void);

/**
* Get the current value of the tick counter.
* Time base in @ref cbHW_getTickFrequency.
* @note The value may wrap.
*
* @return The current tick counter.
*/
cb_uint32 cbHW_getTicks(void);

/**
* Enter forced boot mode. The bootloader will start in x-modem
* mode and emit CCCC.. to notify that it is ready to receive
* a new fw.
* This function will return and boot mode will be entered
* after a device specific timeout.
* @param address x-modem file download start address
* @param baudrate x-modem download buadrate
* @return None
*/
void cbHW_forceBoot(cb_uint32 address, cb_uint32 baudrate);
void cbHW_enterProductionMode(cbHW_FlowControl flowControl);
cbHW_ResetReason cbHW_resetReason(void);
cbHW_FlowControl cbHW_flowControl(void);

/**
* Calculates a hash over a dataset.
* @param type: type of hashing, MD5 for now
* @param pInData: pointer to Data over which the hashing should be done
* @param indataSize: size of data buffer.
* @param pOutData: pointer to result data
* @return None
*/
void cbHW_hash(cbHW_HashType type, const cb_uint8* pInData,cb_uint32 indataSize, cb_uint8* pOutData);

#ifdef __cplusplus
}
#endif

#endif


