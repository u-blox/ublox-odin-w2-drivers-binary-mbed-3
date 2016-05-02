/*------------------------------------------------------------------------------
* Copyright (c) 2016 u-blox AB, Sweden.
* Any reproduction without written permission is prohibited by law.
*------------------------------------------------------------------------------
* Component: Ethernet driver
* File     :
* Revision :
*
* Date     :
* Author   :
*------------------------------------------------------------------------------
* Description: This declaration file contains all constants, types, macros and
* interface definitions for the Ethernet driver
*------------------------------------------------------------------------------
*/
  

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "cb_comdefs.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO add documentation

typedef void(*cbETH_packetIndication)(cb_uint8* pBuf, cb_uint32 length);

// Weak callbacks from HAL that can be overridden
typedef void(*cbETH_HAL_ETH_RxCpltCallback)(ETH_HandleTypeDef *heth);
typedef void(*cbETH_IRQHandlerCallback)(void);
typedef void(*cbETH_MspInitCallback)(ETH_HandleTypeDef *heth);
typedef void(*cbETH_MspDeInitCallback)(ETH_HandleTypeDef *heth);

typedef struct {
    cbETH_HAL_ETH_RxCpltCallback rxCpltCallback;
    cbETH_IRQHandlerCallback iRQHandler;
    cbETH_MspInitCallback mspInit;
    cbETH_MspDeInitCallback mspDeInit;
} cbETH_HALWeakCallbacks;

void cbETH_init(cb_uint8* pMacAddr, cbETH_packetIndication packetInfo);

cb_boolean cbETH_transmit(cb_uint32 len);
cb_uint8* cbETH_getTransmitBuffer(void);

// Override weak callbacks from HAL. Used when an alternate Ethernet driver shall be used.
void cbETH_overrideHALWeakCallbacks(cbETH_HALWeakCallbacks* pCallbacks);

#ifdef __cplusplus
}
#endif

#endif
