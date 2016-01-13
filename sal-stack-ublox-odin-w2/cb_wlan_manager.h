#ifndef _CB_WLAN_MANAGER_H_
#define _CB_WLAN_MANAGER_H_

/*---------------------------------------------------------------------------
 * Copyright (c) 2014 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component   : Application
 * File        : cb_btm.h
 *
 * Description : WLAN manager
 *-------------------------------------------------------------------------*/

#include "cb_status.h"
//#include "cb_url.h"
#include "cb_wlan_types.h"
/*===========================================================================
 * DEFINES
 *=========================================================================*/
#define cbWLANMGR_SSID_MAX_LENGTH (32)
/*===========================================================================
 * TYPES
 *=========================================================================*/
typedef enum
{
    cbWLANMGR_STATUS_STARTED,
    cbWLANMGR_STATUS_DISCONNECTED,
    cbWLANMGR_STATUS_CONNECTED
} cbWLANMGR_statusIndicationInfo;

typedef void (*cbWLANMGR_networkStatusIndication)(cbWLANMGR_statusIndicationInfo status, void* data);

typedef struct cbWLANMGR_Ssid_s {
    cb_uint8 ssid[cbWLANMGR_SSID_MAX_LENGTH];
    cb_uint32 ssidLength;
} cbWLANMGR_Ssid;

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/
void cbWLANMGR_init(cb_uint8* pMacAddr);
void cbWLANMGR_registerWlanStatusIndicationCallback(cbWLANMGR_networkStatusIndication callback);
void cbWLANMGR_enable(cb_boolean enable);
void cbWLANMGR_setConfigSsid(cb_char* ssid);
void cbWLANMGR_setConfigWpa2(cb_char* passPhrase);
void cbWLANMGR_setConfigOpen(void);

#endif /*_CB_WLAN_MANAGER_H_*/
