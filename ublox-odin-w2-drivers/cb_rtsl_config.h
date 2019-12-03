#ifndef _CB_RTSL_CONFIG_H_
#define _CB_RTSL_CONFIG_H_

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
#if defined(RTSL_CONFIG_FILE)
#include RTSL_CONFIG_FILE
#else
TODO("Add in mk the following to specify device override: DEFS+=RTSL_CONFIG_FILE=\"<nina_w1_rtsl_config.h>\"  ")
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define CB_RTSL_VERSION             1

#ifndef cbOS_MAX_ALLOC_SIZE
#define cbOS_MAX_ALLOC_SIZE         (2000u)
#endif

/* cb_heap configuration */
#ifdef CB_HEAP
#ifndef cbHEAP_SIZE
#define cbHEAP_SIZE                 (48200u)
#endif
#ifndef cbHEAP_N_BUFFER_SIZES
#define cbHEAP_N_BUFFER_SIZES       (4u)
#endif
#ifndef cbHEAP_BUFFER_CONFIG
#define cbHEAP_BUFFER_CONFIG        {68,492,1052,1720}
#endif
/* cb_heap configuration */
#ifndef cbHEAPSTATIC_SIZE
#define cbHEAPSTATIC_SIZE(22 * 1024)
#endif
#else /* CB_HEAP */

// Using cb_lwip_heap.c and lwip heap, see lwippools.h and cc.h
// We are using fast_malloc for not DMA-able data

#define UVISOR_BSS_RESERVED_SIZE         (8192) 

#ifndef cbHEAP_FAST_SIZE
#define cbHEAP_FAST_SIZE                 (65520 - UVISOR_BSS_RESERVED_SIZE) // Matching ODIN-W2 CCM memory size
#endif
#ifndef cbHEAP_FAST_N_BUFFER_SIZES
#define cbHEAP_FAST_N_BUFFER_SIZES       (16)
#endif
#ifndef cbHEAP_FAST_BUFFER_CONFIG
#define cbHEAP_FAST_BUFFER_CONFIG        {16,32,56,104,144,200, 320,560,720,1200,1600,2500,3200,4200,8196,16736}
#endif
#endif /* CB_HEAP */

#ifdef __GNUC__

#ifndef cbHEAP_SECTION_CONFIG_INLINE
#  define cbHEAP_SECTION_CONFIG_INLINE          __attribute__((section (".cb_heap_data")))
#endif
#ifndef cbHEAP_FAST_SECTION_CONFIG_INLINE
#  define cbHEAP_FAST_SECTION_CONFIG_INLINE    __attribute__((section (".cb_fast_heap")))
#endif
#else

#ifndef cbHEAP_SECTION_CONFIG_INLINE
#  define cbHEAP_SECTION_CONFIG_INLINE
#endif
#ifndef cbHEAP_FAST_SECTION_CONFIG_INLINE
#  define cbHEAP_FAST_SECTION_CONFIG_INLINE
#endif
#endif

/* NVDS configuration */
#ifndef cbNVDS_SECTOR_LIST
#define cbNVDS_SECTOR_LIST              0x081c0000, 0x081e0000
#endif
#ifndef cbNVDS_NBR_OF_SECTORS
#define cbNVDS_NBR_OF_SECTORS           (2)
#endif
#ifndef cbNVDS_SECTOR_SIZE
#define cbNVDS_SECTOR_SIZE              0x20000
#endif
/* cb_timer configuration*/
#ifndef cbTIMER_MAX
#define cbTIMER_MAX                 (42)
#endif
#ifndef cbTIMER_RESOLUTION_MS
#define cbTIMER_RESOLUTION_MS       ((cb_uint32)1)
#endif

/* cb_led configuration */
#ifndef cbLED_ACTIVE_LOW
#define cbLED_ACTIVE_LOW TRUE
#endif
/* cb_button configuration */
#ifndef cbBUTTON_MAX_NR_BUTTONS
#define cbBUTTON_MAX_NR_BUTTONS 2
#endif
/* cb_fs configuration */
#ifndef cbFS_NVDS_BEGIN
#define cbFS_NVDS_BEGIN                   (65536)
#endif
#ifndef cbFS_NVDS_END
#define cbFS_NVDS_END                     (65536000)
#endif
#ifndef cbFS_NBR_FILE_INODE_EXPONENT
#define cbFS_NBR_FILE_INODE_EXPONENT      (16)   //Total size for each file will be ((1 << cbFS_NBR_FILE_INODE_EXPONENT) - 1) * size of block
#endif
/* cb_otp configuration */

#ifndef cbOTP_MAX_SIZE
#define cbOTP_MAX_SIZE                    (30)
#endif

#endif /* _CB_RTSL_CONFIG_TEMPLATE_H_ */
