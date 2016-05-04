#ifndef __CB_TEMP_SERVICE_H__
#define __CB_TEMP_SERVICE_H__

/*
* PackageLicenseDeclared: Apache-2.0
* Copyright (c) 2016 u-blox
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "cb_comdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * CONSTANTS
 *==============================================================================
 */

#define cbGATT_SERVICE_UBLOX_TEMP              0xFFE0
#define cbGATT_CHAR_UBLOX_TEMP                 0xFFE1

#define cbTEMP_SERVICE_OK     0
#define cbTEMP_SERVICE_ERROR -1

/*==============================================================================
 * TYPES
 *==============================================================================
 */

/*=============================================================================
 * EXPORTED FUNCTIONS
 *=============================================================================
 */

extern void cbTEMP_SERVICE_set(cb_int8 temp);

extern void  cbTEMP_SERVICE_init(void);

extern cb_int32 cbTEMP_SERVICE_register(cb_uint16 startIndex);

extern cb_int32 cbTEMP_SERVICE_getNbrOfAttributes(void);

#ifdef __cplusplus
}
#endif

#endif
