/*---------------------------------------------------------------------------
 * Copyright (C) u-blox
 *
 * u-blox reserves all rights in this deliverable (documentation, software,
 * etc., hereafter "Deliverable").
 *
 * This file is the sole property of u-blox. Its reproduction, modification,
 * re-distribution, sub-licensing or any other use is subject to u-blox's
 * General Terms and Conditions of Sales or to specific terms applicable to
 * the file as indicated by u-blox.
 *
 * Component   : Assert
 * File        : cb_assert.h
 *
 * Description : ASSERT macro variations.
 *-------------------------------------------------------------------------*/

#ifndef _CB_ASSERT_H_
#define _CB_ASSERT_H_

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
extern void cbOS_hwReset(void);
#ifndef NASSERT

#ifndef __CB_FILE__
    #define __CB_FILE__ __FILE__
#endif

/*
GENERATE_ASSERT_RESET can be overridden with hardware specific reset options
*/
#ifndef GENERATE_ASSERT_RESET
#define GENERATE_ASSERT_RESET() do { cbOS_hwReset(); while(1); } while(0)
#endif
/*
 * If the condition (C) evaluates to FALSE, the registered error handler in cbOS
 * is called with file and line info before the system is reset.
 */
#define cb_ASSERTV2(C, FILE, LINE)                  \
    do {                                            \
        if (!(C)) {                                 \
            cbOS_error2(FILE, LINE);                \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT(C)                                \
    do {                                            \
        if (!(C)) {                                 \
            cbOS_error2(__CB_FILE__, __LINE__);     \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERTC(C)                               \
    do {                                            \
        if (!(C)) {                                 \
            cbOS_error2(__CB_FILE__, __LINE__);     \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT2(C, E)                            \
    do {                                            \
        if (!(C)) {                                 \
            cbOS_error(E, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_EQUAL(v, test)                    \
    do {                                            \
        if (v != test) {                            \
            cbOS_error(v, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_NOT_EQUAL(v, test)                \
    do {                                            \
        if (v == test) {                            \
            cbOS_error(v, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_GREATER(v, limit)                 \
    do {                                            \
        if (v <= limit) {                           \
            cbOS_error(v, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_GREATER_OR_EQUAL(v, limit)        \
    do {                                            \
        if (v < limit) {                            \
            cbOS_error(v, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_LESS(v, limit)                    \
    do {                                            \
        if (v >= limit) {                           \
            cbOS_error(v, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_LESS_OR_EQUAL(v, limit)           \
    do {                                            \
        if (v > limit) {                            \
            cbOS_error(v, __CB_FILE__, __LINE__);   \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_NOT_NULL(p)                       \
    do {                                            \
        if (p == NULL) {                            \
            cbOS_error2(__CB_FILE__, __LINE__);     \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

#define cb_ASSERT_IS_NULL(p)                        \
    do {                                            \
        if (p != NULL) {                            \
            cbOS_error2(__CB_FILE__, __LINE__);     \
            GENERATE_ASSERT_RESET();                \
        }                                           \
    } while(0)                                      \

/*
 * The registered error handler is called with the file and line info before a system reset.
 */

#define cb_EXIT(E)                                           \
    do {                                                     \
        cbOS_error(((cb_int32)(E)), __CB_FILE__, __LINE__);  \
        GENERATE_ASSERT_RESET();                             \
    } while(0)                                               \


#else

#define cb_ASSERT(C)

#define cb_ASSERTC(C)                                 \
    do {                                              \
        if (!(C)) {                                   \
            cbWD_systemReset();                       \
        }                                             \
    } while(0) /* Critical assert is never removed */ \

#define cb_ASSERT2(C, E)

#define cb_EXIT(E)          \
    do {                    \
        cbWD_systemReset(); \
    } while(0)              \

#endif


/*===========================================================================
 * TYPES
 *=========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _cb_ASSERT_H_ */





