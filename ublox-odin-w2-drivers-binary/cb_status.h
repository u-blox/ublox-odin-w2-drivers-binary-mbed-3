/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : RTSL
 * File        : cb_status.h
 *
 * Description : Common RTSL status codes 
 *-------------------------------------------------------------------------*/
#ifndef _CB_STATUS_H_
#define _CB_STATUS_H_

/*===========================================================================
 * DEFINES
 *=========================================================================*/

#define OK(status) (status == cbSTATUS_OK)
 
/*===========================================================================
 * TYPES
 *=========================================================================*/

 typedef enum
 {
    cbSTATUS_OK,
    cbSTATUS_ERROR,
    cbSTATUS_BUSY,
    cbSTATUS_TIMEOUT
 
 } cbRTSL_Status;

#endif /* _CB_STATUS_H_ */

