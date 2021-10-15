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
 * Component   : RTSL
 * File        : cb_log.h
 *
 * Description : Provides general mechanism to print strings. 
 *-------------------------------------------------------------------------*/
#ifndef _CB_LOG_H_
#define _CB_LOG_H_

#include "cb_comdefs.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/

#ifdef CB_DEBUG_LOG
#define cbLOG_PRINT(...)                    cbLOG_print(__LINE__,__CB_FILE__, __VA_ARGS__ )
#define cbLOG_WARNING(...)                  cbLOG_warning(cbLOG_COLOR_YELLOW,__LINE__,__CB_FILE__, __VA_ARGS__ )
#define cbLOG_WARNING_WITH_COLOR(color,...) cbLOG_warning(color,__LINE__,__CB_FILE__, __VA_ARGS__ )
#define cbLOG_FUNCTION(a,...)               cbLOG_function(__LINE__,__CB_FILE__,a,TRUE, __VA_ARGS__ )
#define cbLOG_FUNCTION_NO_TS(a,...)         cbLOG_function(__LINE__,__CB_FILE__,a,FALSE, __VA_ARGS__ )
#define cbLOG_CALLBACK(a,...)               cbLOG_callback(__LINE__,__CB_FILE__,a, TRUE,__VA_ARGS__ )
#define cbLOG_CALLBACK_NO_TS(a,...)         cbLOG_callback(__LINE__,__CB_FILE__,a,FALSE, __VA_ARGS__ )
#else
#define cbLOG_PRINT(...)
#define cbLOG_WARNING(...)
#define cbLOG_WARNING_WITH_COLOR(color,...)
#define cbLOG_FUNCTION(a,...)
#define cbLOG_FUNCTION_NO_TS(a,...)
#define cbLOG_CALLBACK(a,...)
#define cbLOG_CALLBACK_NO_TS(a,...)
#endif

/*===========================================================================
 * TYPES
 *=========================================================================*/

/* The print handler is installed by calling the cbLOG_registerPrintHandler.
 * When the cbLOG_print function is called, the print handler is invoked.
 * Hence, the implementation of a print is user defined. The input to the
 * print handler is the same as for the ANSI C vprintf function.
 */
typedef enum
{
    cbLOG_LEVEL_0, // Logging is turned off
    cbLOG_LEVEL_1, // Only errors are logged
    cbLOG_LEVEL_2, // All functions/callbacks except data are logged
    cbLOG_LEVEL_3, // All functions/callbacks including data are logged
} cbLOG_Level;

typedef enum
{
    cbLOG_COLOR_BLACK  = 0,
    cbLOG_COLOR_BLUE = 1,
    cbLOG_COLOR_GREEN = 2,
    cbLOG_COLOR_CYAN = 3,
    cbLOG_COLOR_RED = 4,
    cbLOG_COLOR_MAGENTA = 5,
    cbLOG_COLOR_BROWN = 6,
    cbLOG_COLOR_LIGHTGREY = 7,
    cbLOG_COLOR_DARKGREY = 8,
    cbLOG_COLOR_LIGHTBLUE = 9,
    cbLOG_COLOR_LIGHTGREEN = 10,
    cbLOG_COLOR_LIGHTCYAN = 11,
    cbLOG_COLOR_LIGHTRED = 12,
    cbLOG_COLOR_LIGHTMAGENTA = 13,
    cbLOG_COLOR_YELLOW = 14,
    cbLOG_COLOR_WHITE = 15
} cbLOG_Color;

/*---------------------------------------------------------------------------
* Callback that is called when cbLOG_print is invoked.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - fmt: format string, same as for the ANSI C vprintf function
* - ap: argument pointer, same as for the ANSI C vprintf function
*-------------------------------------------------------------------------*/
typedef void (*cbLOG_PrintHandler)(cb_uint32 line, cb_char* filename, const cb_char* fmt, va_list ap);

/*---------------------------------------------------------------------------
* Callback that is called when cbLOG_warning is invoked.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - fmt: format string, same as for the ANSI C vprintf function
* - ap: argument pointer, same as for the ANSI C vprintf function
*-------------------------------------------------------------------------*/
typedef void (*cbLOG_WarningHandler)(cbLOG_Color, cb_uint32 line, cb_char* filename, const cb_char* fmt, va_list ap);

/*---------------------------------------------------------------------------
* Callback that is called when cbLOG_function is invoked.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - level: log level
* - fmt: format string, same as for the ANSI C vprintf function
* - ap: argument pointer, same as for the ANSI C vprintf function
*-------------------------------------------------------------------------*/
typedef void (*cbLOG_FunctionHandler)(cb_uint32 line, cb_char* filename, cbLOG_Level level, cb_boolean timeStamp, const cb_char* fmt, va_list ap);

/*---------------------------------------------------------------------------
* Callback that is called when cbLOG_callback is invoked.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - level: log level
* - fmt: format string, same as for the ANSI C vprintf function
* - ap: argument pointer, same as for the ANSI C vprintf function
*-------------------------------------------------------------------------*/
typedef void (*cbLOG_CallbackHandler)(cb_uint32 line, cb_char* filename, cbLOG_Level level, cb_boolean timeStamp, const cb_char* fmt, va_list ap);

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/*---------------------------------------------------------------------------
 * Initializes the log module
 *-------------------------------------------------------------------------*/
void cbLOG_init(void);

/*---------------------------------------------------------------------------
* Registers the user defined print handler.
* - printHandler: print handler callback
*-------------------------------------------------------------------------*/
void cbLOG_registerPrintHandler(cbLOG_PrintHandler printHandler);

/*---------------------------------------------------------------------------
* Registers the user defined warning handler.
* - warningHandler: warning handler callback
*-------------------------------------------------------------------------*/
void cbLOG_registerWarningHandler(cbLOG_WarningHandler warningHandler);

/*---------------------------------------------------------------------------
* Registers the user defined function print handler.
* - functionHandler: function handler callback
*-------------------------------------------------------------------------*/
void cbLOG_registerFunctionHandler(cbLOG_FunctionHandler functionHandler);

/*---------------------------------------------------------------------------
* Registers the user defined callback print handler.
* - callbackHandler: callback handler callback
*-------------------------------------------------------------------------*/
void cbLOG_registerCallbackHandler(cbLOG_CallbackHandler callbackHandler);

/*---------------------------------------------------------------------------
* Logs a string according to the user installed print handler.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - fmt: format string, same as for the ANSI C printf function
* - ...: arguments, same as for the ANSI C printf function
*-------------------------------------------------------------------------*/
void cbLOG_print(cb_uint32 line, cb_char* filename, const cb_char* fmt, ...);

/*---------------------------------------------------------------------------
* Logs a string according to the user installed warning handler.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - fmt: format string, same as for the ANSI C printf function
* - ...: arguments, same as for the ANSI C printf function
*-------------------------------------------------------------------------*/
void cbLOG_warning(cbLOG_Color color, cb_uint32 line, cb_char* filename, const cb_char* fmt, ...);

/*---------------------------------------------------------------------------
* Logs a string according to the user installed function handler.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - level: log level
* - fmt: format string, same as for the ANSI C printf function
* - ...: arguments, same as for the ANSI C printf function
*-------------------------------------------------------------------------*/
void cbLOG_function(cb_uint32 line, cb_char* filename, cbLOG_Level level, cb_boolean timeStamp, const cb_char* fmt, ...);

/*---------------------------------------------------------------------------
* Logs a string according to the user installed callback handler.
* - line: line number
* - fileCode: file code as defined in cb_assert.h
* - level: log level
* - fmt: format string, same as for the ANSI C printf function
* - ...: arguments, same as for the ANSI C printf function
*-------------------------------------------------------------------------*/
void cbLOG_callback(cb_uint32 line, cb_char* filename, cbLOG_Level level, cb_boolean timeStamp, const cb_char* fmt, ...);

cb_char* cbLOG_dataToHex(const cb_uint8* pData, cb_uint16 len);

cb_char* cbLOG_macAddrToHex(cb_uint8* pAddr);

cb_char* cbLOG_uiToHex(cb_uint16 val);

cb_char* cbLOG_toStr(cb_char* str,cb_uint32 length);

#ifdef __cplusplus
}
#endif

#endif /* _CB_LOG_H_ */

