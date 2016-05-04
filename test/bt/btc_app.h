#ifndef _BTC_APP_H_
#define _BTC_APP_H_

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

#include <stdint.h>
#include "ublox-odin-w2-drivers/bt_types.h"

/*===========================================================================
* DEFINES
*=========================================================================*/

#define BTC_APP_OK       (0)
#define BTC_APP_ERROR    (-1)
#define btcAPP_MAX_CONNECTIONS      (5)

/*===========================================================================
* TYPES
*=========================================================================*/
typedef int32_t btcAPPe;

typedef enum
{
    SPP_CONNECTED = 0,
    STREAM_CONNECTED
} btcAPPconnection_t;

typedef void(*btcAPP_InquiryEvt) (
    btcAPPe status,
    uint16_t nbrOfDevices);

typedef void(*btcAPP_BondEvt) (
    btcAPPe status);

typedef void(*btcAPP_ConnectEvt) (
    btcAPPe status,
    int16_t handle,
    TBdAddr* bdAddr);

typedef void(*btcAPP_DisonnectEvt) (
    int16_t handle,
    TBdAddr* bdAddr);

typedef void(*btcAPP_DataAvailableEvt) (
    int16_t handle);

typedef void(*btcAPP_StreamWriteCnf) (
    uint32_t handle,
    int32_t status,
    uint32_t nBytes,
    int32_t tag);

typedef struct
{
    btcAPP_InquiryEvt       inquiryCnf;
    btcAPP_BondEvt          bondEvent;
    btcAPP_ConnectEvt       connectEvt; //includes openStream
    btcAPP_DisonnectEvt     disonnectEvt;
    btcAPP_DataAvailableEvt dataAvailableEvt;
    btcAPP_StreamWriteCnf   streamWriteCnf;
} btcAPP_EventCallBack;

/*===========================================================================
* FUNCTIONS
*=========================================================================*/
void btcAPPinit(void);
btcAPPe btcAPPinquiry(void);
btcAPPe btcAPPreqBond(TBdAddr *pAddress);
btcAPPe btcAPPregisterCallBacks(btcAPP_EventCallBack* pCallBacks);
btcAPPe btcAPPrequestConnectSPP(TBdAddr *pAddress);
btcAPPe btcAPPwrite(uint32_t handle, uint8_t *pBuf, uint32_t nBytes, int32_t tag);
btcAPPe btcAPPdisconnect(uint32_t handle);

#endif /*_BTC_APP_H_*/