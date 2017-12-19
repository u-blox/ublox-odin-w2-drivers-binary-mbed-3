/**
 ******************************************************************************
  * File Name          : ethernetif.h
  * Date               : 12/05/2014 11:33:37
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * COPYRIGHT(c) 2014 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  ******************************************************************************
  */
  

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "cb_comdefs.h"
// #include "netif.h"
// #include "cmsis_os.h"

/* Exported types ------------------------------------------------------------*/
/* Structure that include link thread parameters */
// struct link_str {
//   struct netif *netif;
//   osSemaphoreId semaphore;
// };

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


typedef enum
{
    cbETH_PHYSUPPORT_DISABLE = 0,
    cbETH_PHYSUPPORT_ENABLE = 1
}cbETH_MODE;

typedef enum
{
    cbETH_100M_SPEED = 0,
    cbETH_10M_SPEED = 1
}cbETH_SPEED;

typedef enum
{
    cbETH_FULL_DUPLEX = 0,
    cbETH_HALF_DUPLEX = 1
}cbETH_DUPLEX;

typedef enum
{
    cbETH_AUTONEGOTIATION_DISABLE = 0,
    cbETH_AUTONEGOTIATION_ENABLE = 1
}cbETH_AUTONEGOTIATION;

typedef enum
{
    cbETH_PHYSTATUS_LINKDOWN,
    cbETH_PHYSTATUS_LINKUP,
    cbETH_PHYSTATUS_DOWN
} cbETH_PhyStatus;

typedef cb_uint8 cbETH_MACAddress[6];

typedef struct cbETH_config
{
    cb_uint16 MTU;
    cbETH_MACAddress macAddress;
    cb_uint8 phyAddress;
    cbETH_MODE mode;
    cbETH_SPEED speed;
    cbETH_DUPLEX duplex;
    cbETH_AUTONEGOTIATION autonegotiation;
} cbETH_Config;

/* Exported functions ------------------------------------------------------- */
// err_t ethernetif_init(struct netif *netif);
typedef void(*cbETH_packetIndication)(cb_uint8* pBuf, cb_uint32 length);
typedef void(*cbETH_ifStatusIndication)(cbETH_PhyStatus status);

cb_boolean cbETH_init(cbETH_Config *ethConfig, cbETH_packetIndication packetInfo, cbETH_ifStatusIndication ifStatusCallback);

void ethernetif_input( void const * argument ); 
 

void ethernetif_set_link(void const *argument);
void ETHERNET_IRQHandler(void);
// void ethernetif_update_config(struct netif *netif);
// void ethernetif_notify_conn_changed(struct netif *netif);

/* USER CODE BEGIN 1 */
cb_boolean cbETH_transmit(cb_uint32 len);
cb_uint8* cbETH_getTransmitBuffer(void);

/* USER CODE END 1 */
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
