#ifndef __CB_GATT_SERVER_H__
#define __CB_GATT_SERVER_H__
/*------------------------------------------------------------------------------
* Copyright (c) 2016 u-blox AB, Sweden.
* Any reproduction without written permission is prohibited by law.
*------------------------------------------------------------------------------
* Component: GATT
* File     : cb_gatt_server.h
*
* Description:
* GATT server functionality
*------------------------------------------------------------------------------
*/

/**
 * @cb_gatt_server.h
 *
 * @brief GATT server functionality
 */

#include "bt_types.h"
#include "cb_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * CONSTANTS
 *==============================================================================
 */

#define cbGATT_RD_SEC_NONE     0x01
#define cbGATT_RD_SEC_UNAUTH   0x02
#define cbGATT_RD_SEC_AUTH     0x04
#define cbGATT_WR_SEC_NONE     0x10
#define cbGATT_WR_SEC_UNAUTH   0x20
#define cbGATT_WR_SEC_AUTH     0x40

#define cbGATT_NBR_OF_ATTR_OF_SERVICE(x)    (sizeof(x)/sizeof(cbGATT_Attribute))

/*==============================================================================
 * TYPES
 *==============================================================================
 */

typedef struct
{
    void*               pUuid;
    cbGATT_UuidFormat   uuidFormat;
    cb_uint8            properties;
    cb_uint8            security;
    void*               pvValue1;
    void*               pvValue2;
    cb_uint16*          pAttrHandle;
} cbGATT_Attribute;


typedef void (*cbGATT_CharacteristicValueIndicationCnf)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cbGATT_ErrorCode    errorCode);

typedef void (*cbGATT_CharacteristicValueNotificationCnf)(
    TConnHandle         handle,
    cbGATT_ErrorCode    errorCode);

typedef cbGATT_ErrorCode (*cbGATT_CharacteristicWriteLongCommitEvt)(
    TConnHandle         handle,
    cb_boolean          commit);

// Callbacks to use in server table
typedef cbGATT_ErrorCode (*cbGATT_ServerReadAttr)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cbGATT_Attribute*   pAttr,
    cb_uint8*           pAttrValue,
    cb_uint16*          pLength,
    cb_uint16           maxLength,
    cb_uint16           offset);

typedef cbGATT_ErrorCode (*cbGATT_ServerWriteAttr)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cbGATT_Attribute*   pAttr,
    cb_uint8*           pAttrValue,
    cb_uint16           length,
    cbGATT_WriteMethod  writeMethod,
    cb_uint16           offset);

typedef cbGATT_ErrorCode (*cbGATT_ServerReadClientConfig)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cb_uint16*          pConfig);

typedef cbGATT_ErrorCode (*cbGATT_ServerWriteClientConfig)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cb_uint16           config,
    cbGATT_WriteMethod  writeMethod);

typedef cbGATT_ErrorCode (*cbGATT_ServerReadServerConfig)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cb_uint16*          pConfig);

typedef cbGATT_ErrorCode (*cbGATT_ServerWriteServerConfig)(
    TConnHandle         handle,
    cb_uint16           attrHandle,
    cb_uint16           config,
    cbGATT_WriteMethod  writeMethod);

typedef struct
{
    cbGATT_ConnComplEvt connComplEvt;
    cbGATT_DisconnectEvt disconnectEvt;
    cbGATT_CharacteristicValueIndicationCnf characteristicValueIndicationCnf;
    cbGATT_CharacteristicValueNotificationCnf characteristicValueNotificationCnf;
    cbGATT_CharacteristicWriteLongCommitEvt characteristicWriteLongCommitEvt;
} cbGATT_ServerCallBack;


/*=============================================================================
 * EXPORTED FUNCTIONS
 *=============================================================================
 */

cb_int32 cbGATT_registerServer(
    const cbGATT_ServerCallBack* pCallBack, 
    cb_uint8*       pAppHandle);

cb_int32 cbGATT_deregisterAllServers(void);

cb_int32 cbGATT_notification(
    TConnHandle     handle, 
    cb_uint16       attrHandle, 
    cb_uint8*       pData, 
    cb_uint16       length, 
    cb_uint8        appHandle);

cb_int32 cbGATT_indication(
    TConnHandle     handle, 
    cb_uint16       attrHandle, 
    cb_uint8*       pData, 
    cb_uint16       length, 
    cb_uint8        appHandle);

cb_int32 cbGATT_confirmation(
    TConnHandle     handle, 
    cb_uint8        appHandle);

cb_int32 cbGATT_writeRsp(
    TConnHandle     handle,
    cb_uint16       attrHandle, 
    cb_uint8        errorCode); // For delayed write responses

cb_int32 cbGATT_allocServices(
    cb_uint16       attrListSize);

// Note that startHandle for the application should start at lowest 1024. 1-1023 is reserved for GATT/GAP and other u-blox services
cb_int32 cbGATT_addService(
    const cbGATT_Attribute* pAttrList, 
    cb_uint16       attrListSize, 
    cb_int16        startHandle);

cb_int32 cbGATT_freeServices(void);

#ifdef __cplusplus
}
#endif

#endif
