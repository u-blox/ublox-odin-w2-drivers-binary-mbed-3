#ifndef _CB_RTSL_CONFIG_H_
#define _CB_RTSL_CONFIG_H_

/*---------------------------------------------------------------------------
 * Copyright (c) 2014 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : RTSL
 * File        : cb_rtsl_config.h
 *
 * Description : Application specific RTSL configuration.
 *               Test application uses default configuration
 *-------------------------------------------------------------------------*/

#include "cb_comdefs.h"
#include "cb_rtsl_config_template.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/* cb_heap configuration. There is no reason the default configuration
 * is not used but this an example of how reconfiguration of default
 * parameters is done.
 */

// TODO remove 8192 when uvisor has been removed
#undef cbHEAP_FAST_SIZE
#define cbHEAP_FAST_SIZE                 ((65530-8192) - (cbHEAPSTATIC_SIZE)) // Matching ODIN-W2 CCM memory size

#undef cbTIMER_RESOLUTION_MS
#define cbTIMER_RESOLUTION_MS       ((cb_uint32)25)

#endif /* _CB_RTSL_CONFIG_H_ */
