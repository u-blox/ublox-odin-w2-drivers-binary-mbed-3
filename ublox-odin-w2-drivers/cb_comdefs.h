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
 * Component   : Common Definitions
 * File        : cb_comdefs.h
 *
 * Description : Common definitions.
 *-------------------------------------------------------------------------*/

#ifndef _CB_COMDEFS_H_
#define _CB_COMDEFS_H_

#include "cb_platform_basic_types.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

#ifndef FALSE
#  define FALSE                 (0)
#endif

#ifndef TRUE
#  define TRUE                  (!FALSE)
#endif

#ifndef NULL
#  define NULL                  ((void*)0)
#endif

/**
 * Returns the maximum value of the two parameters.
 */
#define cb_MAX(a,b)              (((a) > (b)) ? (a) : (b))

/**
 * Returns the minimum value of the two parameters.
 */
#define cb_MIN(a,b)              (((a) < (b)) ? (a) : (b))

/**
 * Used in function definitions to declare an input parameter unused to avoid warnings.
 */
#ifndef cb_UNUSED
#  define cb_UNUSED(x) x
#endif


/**
 * Used when declaring an empty array that does not take up space in a struct.
 * Example: struct { cb_uint8 payload[cb_EMPTY_ARRAY]; }
 * In some compilers this is empty i.e. payload[]. While in some it requires a zero.
 * I.e. payload[0];
 * Use this define to get it working for your system.
 */
#ifndef cb_EMPTY_ARRAY
#  define cb_EMPTY_ARRAY  (0)
#endif


#define cb_BIT_0        (1ul)
#define cb_BIT_1        (1ul << 1)
#define cb_BIT_2        (1ul << 2)
#define cb_BIT_3        (1ul << 3)
#define cb_BIT_4        (1ul << 4)
#define cb_BIT_5        (1ul << 5)
#define cb_BIT_6        (1ul << 6)
#define cb_BIT_7        (1ul << 7)
#define cb_BIT_8        (1ul << 8)
#define cb_BIT_9        (1ul << 9)
#define cb_BIT_10       (1ul << 10)
#define cb_BIT_11       (1ul << 11)
#define cb_BIT_12       (1ul << 12)
#define cb_BIT_13       (1ul << 13)
#define cb_BIT_14       (1ul << 14)
#define cb_BIT_15       (1ul << 15)
#define cb_BIT_16       (1ul << 16)
#define cb_BIT_17       (1ul << 17)
#define cb_BIT_18       (1ul << 18)
#define cb_BIT_19       (1ul << 19)
#define cb_BIT_20       (1ul << 20)
#define cb_BIT_21       (1ul << 21)
#define cb_BIT_22       (1ul << 22)
#define cb_BIT_23       (1ul << 23)
#define cb_BIT_24       (1ul << 24)
#define cb_BIT_25       (1ul << 25)
#define cb_BIT_26       (1ul << 26)
#define cb_BIT_27       (1ul << 27)
#define cb_BIT_28       (1ul << 28)
#define cb_BIT_29       (1ul << 29)
#define cb_BIT_30       (1ul << 30)
#define cb_BIT_31       (1ul << 31)

/**
 * Clears (set to zero) a bit or bits in a variable.
 * @param variable The variable.
 * @param bit The bit or bits to clear
 */
#define cb_CLEAR_BIT(variable,bit) ((variable) &= ~((bit)))

/**
 * Gets a bit i.e. checks if it is set in a variable.
 *
 * Also works to see if any of several bits are set.
 *
 * @param variable The variable.
 * @param bit The bit to check if it set.
 * @return @ref TRUE if any of the bits are set, @ref FALSE otherwise.
 */
#define cb_GET_BIT(variable,bit) (((variable) & ((bit))) ? TRUE : FALSE)

/**
 * Calculate the number of elements in an array.
 *
 * @note Won't work on pointer to array as the sizeof(pointer) is 4.
 *
 * @param _array The array.
 * @return Number of elements in array.
 */
#define ELEMENTS_OF(_array) (sizeof((_array)) / sizeof((_array)[0]))

/**
 * Sets (set to 1) a bit or bits in a variable.
 *
 * @param variable The variable.
 * @param bit The bit or bits to set in the variable.
 */
#define cb_SET_BIT(variable,bit) ((variable) |= (bit))

#define cb_UINT8_MAX    ((cb_uint8)0xff)
#define cb_UINT16_MAX   ((cb_uint16)0xffff)
#define cb_UINT32_MAX   ((cb_uint32)0xffffffff)
#define cb_INT8_MAX     ((cb_int8)0x7f)
#define cb_INT16_MAX    ((cb_int16)0x7fff)
#define cb_INT32_MAX    ((cb_int32)0x7fffffff)
#define cb_INT8_MIN     ((cb_int8)0x80)
#define cb_INT16_MIN    ((cb_int16)0x8000)
#define cb_INT32_MIN    ((cb_int32)0x80000000)

/* Packed struct default defines */
#ifndef cb_PACKED_STRUCT_ATTR_INLINE_POST
#  define cb_PACKED_STRUCT_ATTR_INLINE_POST
#endif
#ifndef cb_PACKED_STRUCT_ATTR_INLINE_PRE
#  define cb_PACKED_STRUCT_ATTR_INLINE_PRE
#endif
#ifndef cb_PACKED_STRUCT_ATTR_PRE
#  define cb_PACKED_STRUCT_ATTR_PRE
#endif
#ifndef cb_PACKED_STRUCT_ATTR_POST
#  define cb_PACKED_STRUCT_ATTR_POST
#endif

#define cb_PACKED_STRUCT_BEGIN(name) \
    cb_PACKED_STRUCT_ATTR_PRE \
    typedef cb_PACKED_STRUCT_ATTR_INLINE_PRE struct name

#define cb_PACKED_STRUCT_END(name) \
    cb_PACKED_STRUCT_ATTR_INLINE_POST name; \
    cb_PACKED_STRUCT_ATTR_POST

#ifdef __GNUC__
#  define DO_PRAGMA(x) _Pragma (#x)
#  define TODO(x) DO_PRAGMA(message ("TODO - " #x))
#else
#  define TODO(x)
#endif

#ifndef cb_FALLTHROUGH
#  if defined(__GNUC__) & __GNUC__ >= 7
#    define cb_FALLTHROUGH __attribute__((fallthrough)) // defined in gcc v7
#  else
#    define cb_FALLTHROUGH ((void)0) // avoid empty declaration warning in gcc less than v7
#  endif
#endif

/*===========================================================================
 * TYPES
 *=========================================================================*/

#endif /* _cb_COMDEFS_H_ */

// TODO: Assert inclusion is put here for legacy reason; there are a lot of files using asserts that don't include cb_assert.h
#include "cb_assert.h"
