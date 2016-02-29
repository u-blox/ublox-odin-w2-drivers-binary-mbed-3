#ifndef __cbBNEP_H__
#define __cbBNEP_H__

/*---------------------------------------------------------------------------
* Copyright (c) 2016 u-blox AB, Sweden.
* Any reproduction without written permission is prohibited by law.
*
* Component   : BNEP
* File        : cb_bnep.h
*
* Description : External interface for BNEP
*-------------------------------------------------------------------------*/

#include "cb_comdefs.h"
#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * DEFINES
 *=========================================================================*/

#ifndef cbBNEP_MAX_LINKS
#define cbBNEP_MAX_LINKS (7)
#endif

// Max Ethernet frame size
// header (14) + payload (1500) + 802.1Q Tag header (4) => 1518
#define cbBNEP_MAX_FRAME_SIZE               ((cb_uint32)1518)

#define cbBNEP_RESULT_SUCCESS               ((cb_int32)0x00000000)
#define cbBNEP_RESULT_ERROR                 ((cb_int32)0x00000001)
#define cbBNEP_RESULT_ILLEGAL_HANDLE        ((cb_int32)0x00000002)
#define cbBNEP_RESULT_FLOW_STOP             ((cb_int32)0x00000003)
#define cbBNEP_RESULT_LINK_LOSS             ((cb_int32)0x00000004)

#define cbBNEP_ILLEGAL_CONN_HANDLE          (INVALID_CONN_HANDLE)
#define cbBNEP_ILLEGAL_BNEP_HANDLE          ((cb_uint32)0xFFFFFFFF)

/*===========================================================================
 * TYPES
 *=========================================================================*/
typedef cb_uint32   cbBNEP_Handle;

/*---------------------------------------------------------------------------
* Callback to indicate the the BNEP channel is disconnected.
* - bnepHandle: BNEP handle
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_DisconnectEvt)       (cbBNEP_Handle bnepHandle);

/*---------------------------------------------------------------------------
* Callback to indicate that the remote device wants to set up an BNEP
* channel
* - connHandle: Connection handle
* - bnepHandle: BNEP handle
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_ConnectInd)          (TConnHandle connHandle, cbBNEP_Handle bnepHandle);

/*---------------------------------------------------------------------------
* Callback to indicate a new BNEP channel is connected.
* - connHandle: Connection handle
* - bnepHandle: BNEP handle
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_ConnectEvt)          (TConnHandle connHandle, cbBNEP_Handle bnepHandle);

/*---------------------------------------------------------------------------
* Callback to indicate a new BNEP channel is connected.
* - connHandle: Connection handle
* - bnepHandle: BNEP handle
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_ConnectCnf)          (TConnHandle connHandle, cbBNEP_Handle bnepHandle);

/*---------------------------------------------------------------------------
* Callback to indicate that the BNEP channel could not be set up.
* - connHandle: Connection handle
* - errorCode: Error code, e.g. cbBNEP_RESULT_ERROR
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_ConnectCnfNeg)       (TConnHandle connHandle, cb_int32 errorCode);

/*---------------------------------------------------------------------------
* Callback to indicate that data has been received from remote device.
* - bnepHandle: BNEP handle
* - length: Length of the data
* - pData: Pointer to the data
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_DataEvt)             (cbBNEP_Handle bnepHandle, cb_uint16 length, cb_uint8* pData);

/*---------------------------------------------------------------------------
* Callback to indicate that data has been taken care by BNEP. New
* data can now be sent on this BNEP channel.
* - bnepHandle: BNEP handle
* - result: cbBNEP_RESULT_SUCCESS if the data sending succeeded
*-------------------------------------------------------------------------*/
typedef void (*cbBNEP_DataCnf)             (cbBNEP_Handle bnepHandle, cb_int32 result);

/* BNEP Channel Administration */
typedef struct
{
  cbBNEP_DisconnectEvt      disconnectEvt;
  cbBNEP_ConnectInd         connectInd;
  cbBNEP_ConnectEvt         connectEvt;
  cbBNEP_ConnectCnf         connectCnf;
  cbBNEP_ConnectCnfNeg      connectCnfNeg;
  cbBNEP_DataEvt            dataEvt;
  cbBNEP_DataCnf            dataCnf;

} cbBNEP_Callback;

/* BNEP Role */
typedef enum
{
    cbBNEP_R_PANU = 8,
    cbBNEP_R_NAP = 9,
    cbBNEP_R_NONE = 255

} cbBNEP_Role;

/*===========================================================================
* FUNCTIONS
*=========================================================================*/

/*---------------------------------------------------------------------------
* Find out if BNEP resource is free for the specified handle.
* Returns TRUE if resource is available.
*-------------------------------------------------------------------------*/
cb_boolean cbBNEPIsHandleFree(cbBNEP_Handle handle);

/*---------------------------------------------------------------------------
* Sets maximum allowed bnep connections
*-------------------------------------------------------------------------*/
void cbBNEPSetMaxLinks(cb_int32 maxLinks);

/*---------------------------------------------------------------------------
* Initializes the Bluetooth BNEP module.
* Returns cbBNEP_RESULT_SUCCESS on success.
*-------------------------------------------------------------------------*/
extern void      cbBNEP_init            (void);

/*---------------------------------------------------------------------------
* Registers callback functions.
* Returns cbBNEP_RESULT_SUCCESS.
* - localBdAddr: Bluetooth address of local device.
* - pCallback: Pointer to structure of callbacks to register.
*-------------------------------------------------------------------------*/
extern cb_int32  cbBNEP_register        (TBdAddr localBdAddr, cbBNEP_Callback *pCallback);

/*---------------------------------------------------------------------------
* Registers the server role the local device.
* Returns cbBNEP_RESULT_SUCCESS.
* - serverRole: PAN role of the local server.
*-------------------------------------------------------------------------*/
extern cb_int32  cbBNEP_registerServer  (cbBNEP_Role serverRole);

/*---------------------------------------------------------------------------
* Creates a new BNEP connection on an existing ACL connection.
* Returns cbBNEP_RESULT_SUCCESS on success.
* - connHandle: ACL connection handle
* - localDeviceRole: PAN role of the local device.
* - remoteDeviceRole: PAN role of the remote device.
* - remoteBdAddr: The Bluetooth address of the remote device.
*-------------------------------------------------------------------------*/
extern cb_int32 cbBNEP_reqConnect(
    TConnHandle     connHandle,
    cbBNEP_Role     localDeviceRole,
    cbBNEP_Role     remoteDeviceRole,
    TBdAddr         remoteBdAddr);

/*---------------------------------------------------------------------------
* Disconnects the BNEP channel.
* Returns cbBNEP_RESULT_SUCCESS.
* - bnepHandle: BNEP handle
*-------------------------------------------------------------------------*/
extern cb_int32 cbBNEP_cmdDisconnect(cbBNEP_Handle bnepHandle);

/*---------------------------------------------------------------------------
* Accepts a BNEP connection by responding on a cbBNEP_ConnectInd callback.
* Returns cbBNEP_RESULT_SUCCESS on success.
* - bnepHandle: BNEP handle
* - bdAddrRemoteDevice: Bluetooth address of the remote device of the ACL link.
*-------------------------------------------------------------------------*/
extern cb_int32 cbBNEP_rspConnect   (cbBNEP_Handle bnepHandle, TBdAddr bdAddrRemoteDevice);

/*---------------------------------------------------------------------------
* Rejects a BNEP connection.
* Returns cbBNEP_RESULT_SUCCESS on success.
* - bnepHandle: BNEP handle
*-------------------------------------------------------------------------*/
extern cb_int32 cbBNEP_rspNegConnect(cbBNEP_Handle bnepHandle);

/*---------------------------------------------------------------------------
* Sends data to the remote device. Note that you have to wait for the
* confirmation callback (cbBNEP_DataCnf) before calling another cbBNEP_reqData.
* Returns cbBNEP_RESULT_SUCCESS on success.
* - bnepHandle: BNEP handle
* - pBuf: Pointer to the data
* - bufSize: Length of the data
*-------------------------------------------------------------------------*/
extern cb_int32 cbBNEP_reqData      (cbBNEP_Handle bnepHandle, cb_uint8* pBuf, cb_uint16 bufSize);

#ifdef __cplusplus
}
#endif

#endif
