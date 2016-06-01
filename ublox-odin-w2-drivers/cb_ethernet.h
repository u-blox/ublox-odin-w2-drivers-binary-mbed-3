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

#ifndef _CB_ETHERNET_H_
#define _CB_ETHERNET_H_

#include "cb_comdefs.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * Initialize ethernet driver.
 * 
 * @param   pMacAddr      Pointer to MAC address. Must not be NULL.
 */
void cbETH_init(cb_uint8* pMacAddr);

/**
 * Register packet indication callback.
 * 
 * @param   packetInfo      Callback for received packets.
 */
void cbETH_registerPacketIndicationCallback(cbETH_packetIndication packetInfo);

/**
 * Get the transmit buffer
 * 
 * @return   Packet buffer where data to send shall be put.
 */
cb_uint8* cbETH_getTransmitBuffer(void);

/**
 * Transmit packet buffer. Call cbETH_getTransmitBuffer
 * before to get the transmit buffer to put data in.
 * 
 * @param    length Length of the ethernet frame to send
 * @return   TRUE on success otherwise FALSE
 */
cb_boolean cbETH_transmit(cb_uint32 length);

/**
 * Override weak callbacks from HAL. Used when an alternate Ethernet driver shall be used.
 * 
 * @param   pCallbacks Pointer to callbacks
 */
void cbETH_overrideHALWeakCallbacks(cbETH_HALWeakCallbacks* pCallbacks);

#ifdef __cplusplus
}
#endif

#endif
