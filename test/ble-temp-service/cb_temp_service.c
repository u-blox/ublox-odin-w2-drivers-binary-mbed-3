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

#include <string.h>

#include "bt_types.h"
#include "cb_comdefs.h"
#include "cb_gatt.h"
#include "cb_gatt_server.h"
#include "cb_gatt_utils.h"
#include "cb_temp_service.h"
#include <string.h>

/*===========================================================================
* DEFINES
*=========================================================================*/
#define _T(x) (void*)(x)

/*===========================================================================
* TYPES
*=========================================================================*/
typedef struct
{
    cb_int8 temp;
}cbTEMPSERVICE_Class;

/*===========================================================================
* DEFINITIONS
*=========================================================================*/

static cbGATT_ErrorCode handleTemp(
    TConnHandle connHandle, 
    cb_uint16 attrHandle,
    cbGATT_Attribute* pAttr,
    cb_uint8* pAttrValue,
    cb_uint16* pLength,
    cb_uint16 maxLength,
    cb_uint16 offset);

cbTEMPSERVICE_Class _admin;

/*===========================================================================
* DECLARATIONS
*=========================================================================*/

static const cbGATT_Attribute _tempService[] =
{
    {_T(cbGATT_PRIMARY_SERVICE_DECL),           cbGATT_UUID_16,    cbGATT_PROP_RD,cbGATT_RD_SEC_NONE | cbGATT_WR_SEC_NONE,_T(cbGATT_SERVICE_UBLOX_TEMP),_T(cbGATT_UUID_16),NULL},

    {_T(cbGATT_CHAR_DECL),                      cbGATT_UUID_16,    cbGATT_PROP_RD,cbGATT_RD_SEC_NONE | cbGATT_WR_SEC_NONE,NULL,NULL,NULL},
    {_T(cbGATT_CHAR_UBLOX_TEMP),                cbGATT_UUID_16,    cbGATT_PROP_RD,cbGATT_RD_SEC_NONE | cbGATT_WR_SEC_NONE,_T(&handleTemp),NULL,NULL},
};

/*===========================================================================
* FUNCTIONS
*=========================================================================*/


cbGATT_ErrorCode handleTemp(TConnHandle connHandle, 
                                             cb_uint16 attrHandle,
                                             cbGATT_Attribute* pAttr,
                                             cb_uint8* pAttrValue,
                                             cb_uint16* pLength,
                                             cb_uint16 maxLength,
                                             cb_uint16 offset)
{
    (void)connHandle;
    (void)attrHandle;
    (void)maxLength;
    (void)pAttr;

    cbGATT_ErrorCode errorCode = cbGATT_ERROR_CODE_OK;

    if(offset < 1)
    {
        *pLength = 1;
        *pAttrValue = _admin.temp;
    }
    else
    {
        errorCode = cbGATT_ERROR_CODE_INVALID_OFFSET;
    }
    return errorCode;
}

void cbTEMP_SERVICE_set(cb_int8 temp)
{
	_admin.temp = temp;
}

void cbTEMP_SERVICE_init(void)
{
	_admin.temp = -127;
}

cb_int32 cbTEMP_SERVICE_register(cb_uint16 startIndex)
{
    cbGATT_addService(_tempService,cbGATT_NBR_OF_ATTR_OF_SERVICE(_tempService),startIndex);

    return cbTEMP_SERVICE_OK;
}

cb_int32 cbTEMP_SERVICE_getNbrOfAttributes(void)
{
    return cbGATT_NBR_OF_ATTR_OF_SERVICE(_tempService);
}
