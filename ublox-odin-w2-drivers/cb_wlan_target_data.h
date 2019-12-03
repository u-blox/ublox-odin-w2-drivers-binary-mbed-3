/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : Wireless LAN driver
 * File        : cb_wlan_target_data.h
 *
 * Description : Port specific data buffer handling (ethernet frames)
 *-------------------------------------------------------------------------*/

/**
 * @file cb_wlan_target_data.h Handles the anonymous port specific packetization 
 * of ethernet frames.
 * @ingroup target
 */

#ifndef _CB_WLANTARGET_DATA_H_
#define _CB_WLANTARGET_DATA_H_

#include "cb_types.h"
#include "cb_target.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef struct cbWLANTARGET_dataFrame cbWLANTARGET_dataFrame;
typedef struct cbWLANTARGET_Handle cbWLANTARGET_Handle;
typedef struct cbTARGET_dataFrame cbTARGET_dataFrame;

   
/**
 * Copy data from frame data memory to buffer.
 *
 * @param buffer            The destination buffer.
 * @param frame             Frame memory pointer (@ref cbWLANTARGET_allocDataFrame).
 * @param size              Number of bytes to copy.
 * @param offsetInFrame     Offset into frame memory.
 * @return                  @ref TRUE if successful, otherwise @ref FALSE.
 */
typedef cb_boolean(*cbWLANTARGET_copyFromDataFrame)(cb_uint8* buffer, cbWLANTARGET_dataFrame* frame, cb_uint32 size, cb_uint32 offsetInFrame);

/**
 * Copy data from buffer to frame data memory.
 *
 * @param frame             Frame memory pointer (@ref cbWLANTARGET_allocDataFrame).
 * @param buffer            The destination buffer.
 * @param size              Number of bytes to copy.
 * @param offsetInFrame     Offset into frame memory.
 * @return                  @ref TRUE if successful, otherwise @ref FALSE.
 */
typedef cb_boolean(*cbWLANTARGET_copyToDataFrame)(cbWLANTARGET_dataFrame* frame, cb_uint8* buffer, cb_uint32 size, cb_uint32 offsetInFrame);

/**
 * Allocate memory in frame data memory.
 *
 * @param size              Number of bytes to allocate.
 * @return                  Pointer to the frame memory.
 * 
 * @ref cbWLANTARGET_freeDataFrame
 */
typedef cbWLANTARGET_dataFrame*(*cbWLANTARGET_allocDataFrame)(cb_uint32 size);

/**
 * Destroy memory in frame data memory.
 *
 * @param frame             Pointer to the frame memory that should be destroyed.
 * @ref cbWLANTARGET_allocDataFrame
 */
typedef void(*cbWLANTARGET_freeDataFrame)(cbWLANTARGET_dataFrame* frame);

typedef cb_uint32(*cbWLANTARGET_getDataFrameSize)(cbWLANTARGET_dataFrame* frame);

typedef cb_uint8(*cbWLANTARGET_getDataFrameTID)(cbWLANTARGET_dataFrame* frame);

typedef struct
{
    cbWLANTARGET_copyFromDataFrame      copyFromDataFrameIndication;
    cbWLANTARGET_copyToDataFrame        copyToDataFrameIndication;
    cbWLANTARGET_allocDataFrame         allocDataFrameIndication;
    cbWLANTARGET_freeDataFrame          freeDataFrameIndication;
    cbWLANTARGET_getDataFrameSize       getDataFrameSizeIndication;
    cbWLANTARGET_getDataFrameTID        getDataFrameTIDIndication;
}cbWLANTARGET_Callback;
    
/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/**
 * Register WLAN target callbacks. This should be done for packetization between
 * the WLAN driver and an IP stack.
 *
 * @param callbacks           Callbacks
 */
void cbWLANTARGET_registerCallbacks(cbWLANTARGET_Callback* callbacks);
void cbTARGET_addRef(cbTARGET_Handle *hTarget, cbTARGET_dataFrame* dataFrame);

#ifdef __cplusplus
}
#endif

#endif
