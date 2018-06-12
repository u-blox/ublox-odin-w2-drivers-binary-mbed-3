#ifndef _CB_RTSL_CONFIG_TEMPLATE_H_
#define _CB_RTSL_CONFIG_TEMPLATE_H_

/*---------------------------------------------------------------------------
 * Copyright (c) 2014 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : RTSL
 * File        : cb_rtsl_config_template.h
 *
 * Description : Configuration template for RTSL.
 *               This configuration files should not be included 
 *               directly by any RTSL files. Instead "cb_rtsl_config.h"
 *               defined by the application shall be included.
 *-------------------------------------------------------------------------*/

#include "cb_comdefs.h"


/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define CB_RTSL_VERSION             1

#define cbOS_MAX_ALLOC_SIZE         (2000u)

/* cb_heap configuration */
#define cbHEAPSTATIC_SIZE                 (22 * 1024)

#ifdef CB_HEAP
#define cbHEAP_SIZE                 (48200u)
#define cbHEAP_N_BUFFER_SIZES       (4u)
#define cbHEAP_BUFFER_CONFIG        {68,492,1052,1720}

#else /* CB_HEAP */

// Using cb_lwip_heap.c and lwip heap, see lwippools.h and cc.h
// We are using fast_malloc for not DMA-able data
#define cbHEAP_FAST_SIZE                 ((65520) - (cbHEAPSTATIC_SIZE)) // Matching ODIN-W2 CCM memory size
#define cbHEAP_FAST_N_BUFFER_SIZES       (9)
#define cbHEAP_FAST_BUFFER_CONFIG        {56,104,144, 320,560,1200,2500,4200,8196}

#endif /* CB_HEAP */

#ifdef __GNUC__
#  define cbHEAP_SECTION_CONFIG_INLINE          __attribute__((section (".cb_heap_data")))
#  define cbHEAPSTATIC_SECTION_CONFIG_INLINE    __attribute__((section (".cb_static_heap")))
#  define cbHEAP_FAST_SECTION_CONFIG_INLINE    __attribute__((section (".cb_fast_heap")))
#else
#  define cbHEAP_SECTION_CONFIG_INLINE
#  define cbHEAPSTATIC_SECTION_CONFIG_INLINE
#  define cbHEAP_FAST_SECTION_CONFIG_INLINE
#endif

/* NVDS configuration */
#define cbNVDS_SECTOR_LIST              0x081c0000, 0x081e0000
#define cbNVDS_NBR_OF_SECTORS           (2)
#define cbNVDS_SECTOR_SIZE              0x20000

/* cb_timer configuration*/
#define cbTIMER_MAX                 (36)
#define cbTIMER_RESOLUTION_MS       ((cb_uint32)1)

/* cb_led configuration */
#define cbLED_ACTIVE_LOW TRUE

/* cb_button configuration */
#define cbBUTTON_MAX_NR_BUTTONS 2

/* cb_fs configuration */
#define cbFS_NVDS_BEGIN                   ( 65536 )
#define cbFS_NVDS_END                     ( 65536000 )  

/* cb_otp configuration */
#define cbOTP_MAX_SIZE                    (30)


#endif /* _CB_RTSL_CONFIG_TEMPLATE_H_ */
