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

#ifdef __cplusplus
extern "C" {
#endif

// TODO add documentation

typedef void(*cbETH_packetIndication)(cb_uint8* pBuf, cb_uint32 length);

void cbETH_init(cb_uint8* pMacAddr, cbETH_packetIndication packetInfo);

void ethernetif_input( void const * argument ); 

void ethernetif_set_link(void const *argument);
void ETHERNET_IRQHandler(void);

cb_boolean cbETH_transmit(cb_uint32 len);
cb_uint8* cbETH_getTransmitBuffer(void);

#ifdef __cplusplus
}
#endif

#endif
