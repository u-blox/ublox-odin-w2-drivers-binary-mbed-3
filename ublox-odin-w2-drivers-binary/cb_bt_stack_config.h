#ifndef _CB_BT_STACK_CONFIG_H_
#define _CB_BT_STACK_CONFIG_H_

/*---------------------------------------------------------------------------
 * Copyright (c) 2016 u-blox AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : Bluetooth Stack Configuration
 * File        : cb_bt_config.h
 *
 * Description : Application specific Bluetooth configuration.
 *               Test application uses default configuration
 *-------------------------------------------------------------------------*/

#include "cb_comdefs.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

#define BT_LE_ENABLED
#define SUPPORT_GATT

#define BT_RADIO 2564
#define HCI_TRANSPORT_LAYER_H4_UART
#define HCI_TRANSPORT_UART_ID             (cbUART_ID_1)


// This affects the size of the data buffer allocated for RFCOMM data in btserial
// Buffer size = cbBSE_TOTAL_CREDITS * ( MAX_RFCOMM_FRAME_SIZE + OVERHEAD)
// Roughly cbBSE_TOTAL_CREDITS * 700 with current LCAP settings.
#define cbBSE_TOTAL_CREDITS                   14
#define cbBSL_TOTAL_CREDITS                   28

#define BT_LE_ENABLED
#define SUPPORT_GATT

#define MAX_ACL_CLASSIC_LINKS_IN_CLASSIC_MODE 7
#define MAX_ACL_CLASSIC_LINKS_IN_DUAL_MODE 7
#define MAX_ACL_LE_LINKS_IN_DUAL_MODE 7

#define cbBNEP_MAX_LINKS (MAX_ACL_CLASSIC_LINKS_IN_CLASSIC_MODE)

#define L2CA_MAX_MTU_RFCOMM (678-4)
#define L2CA_MAX_MTU_SDP (678-4)
#define L2CA_MAX_MTU_BNEP (1691)

#define MAX_SERVICE_RECORDS 8

#define TX_POWER_MAX_LEVEL_LE_ADVERT 4

#define NPKTCTRL
#define cbHW_PCB 965

#define cbLINK_KEYS_MAX_RECORD_SIZE 256


#endif /* _CB_BT_STACK_CONFIG_H_ */






