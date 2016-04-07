/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : Assert
 * File        : cb_assert.h
 *
 * Description : ASSERT macro variations.
 *-------------------------------------------------------------------------*/
#ifndef _cb_ASSERT_H_
#define _cb_ASSERT_H_

#include "cb_comdefs.h"


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*
 * Internal platform function declaration.
 * Shall never be called directly.
 */

extern void cbOS_error(cb_int32 errorCode, const cb_char *file, cb_uint32 line);
extern void cbOS_error2(const cb_char *file, cb_uint32 line);

#ifndef NASSERT

#ifndef __CB_FILE__
    #define __CB_FILE__ __FILE__
#endif

/*
 * If the condition (C) evaluates to FALSE, the registered error handler in cbOS
 * is called with file and line info before the system is reset.
 */

#define cb_ASSERT(C)    do { if(!(C)){cbOS_error2(__CB_FILE__,__LINE__);} } while(0)

#define cb_ASSERTC(C)   do { if(!(C)){cbOS_error2(__CB_FILE__ , __LINE__);} } while(0)

#define cb_ASSERT2(C, E) do { if(!(C)){cbOS_error(E, __CB_FILE__ , __LINE__);} } while(0)

/*
 * The registered error handler is called with the file and line info before a system reset.
 */

#define cb_EXIT(E) do { cbOS_error(((cb_int32)(E)), __CB_FILE__, __LINE__); } while(0)


#else

#define cb_ASSERT(C)

#define cb_ASSERTC(C) do {  if(!(C)){cbWD_systemReset();} } while(0) // Critical assert is never removed.

#define cb_ASSERT2(C, E)

#define cb_EXIT(E) do { cbWD_systemReset(); } while(0)

#endif


/*===========================================================================
 * TYPES
 *=========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _cb_ASSERT_H_ */





