/*---------------------------------------------------------------------------
 * Copyright (c) 2014 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : Wireless LAN driver
 * File        : cb_target_data.h
 *
 * Description : Port specific data buffer handling (ethernet frames)
 *-------------------------------------------------------------------------*/

/**
 * @file cb_target_data.h Handles the anonymous port specific packetization 
 * of ethernet frames.
 * @ingroup target
 */

#ifndef _CB_TARGET_DATA_H_
#define _CB_TARGET_DATA_H_

#include "cb_types.h"
#include "cb_target.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

typedef struct cbTARGET_dataFrame cbTARGET_dataFrame;

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/**
 * Copy data from frame data memory to buffer.
 *
 * @param hTarget           Target context handle.
 * @param buffer            The destination buffer.
 * @param frame             Frame memory pointer (@ref cbTARGET_allocDataFrame).
 * @param size              Number of bytes to copy.
 * @param offsetInFrame     Offset into frame memory.
 * @return                  @ref TRUE if successful, otherwise @ref FALSE.
 */
cb_boolean cbTARGET_copyFromDataFrame(cbTARGET_Handle *hTarget, cb_uint8* buffer, cbTARGET_dataFrame* frame, cb_uint32 size, cb_uint32 offsetInFrame);

/**
 * Copy data from buffer to frame data memory.
 *
 * @param hTarget           Target context handle.
 * @param frame             Frame memory pointer (@ref cbTARGET_allocDataFrame).
 * @param buffer            The destination buffer.
 * @param size              Number of bytes to copy.
 * @param offsetInFrame     Offset into frame memory.
 * @return                  @ref TRUE if successful, otherwise @ref FALSE.
 */
cb_boolean cbTARGET_copyToDataFrame(cbTARGET_Handle *hTarget, cbTARGET_dataFrame* frame, cb_uint8* buffer, cb_uint32 size, cb_uint32 offsetInFrame);

/**
 * Allocate memory in frame data memory.
 *
 * @param hTarget           Target context handle.
 * @param size              Number of bytes to allocate.
 * @return                  Pointer to the frame memory.
 * 
 * @ref cbTARGET_freeDataFrame
 */
cbTARGET_dataFrame* cbTARGET_allocDataFrame(cbTARGET_Handle *hTarget, cb_uint32 size);

/**
 * Destroy memory in frame data memory.
 *
 * @param hTarget           Target context handle.
 * @param frame             Pointer to the frame memory that should be destroyed.
 * @ref cbTARGET_allocDataFrame
 */
void cbTARGET_freeDataFrame(cbTARGET_Handle *hTarget, cbTARGET_dataFrame* frame);

cb_uint32 cbTARGET_getDataFrameSize(cbTARGET_Handle *hTarget, cbTARGET_dataFrame* frame);

cb_uint8 cbTARGET_getDataFrameTID(cbTARGET_Handle *hTarget, cbTARGET_dataFrame* frame);

#endif
