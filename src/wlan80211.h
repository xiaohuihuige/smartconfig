#ifndef __WLAN_80211_H__
#define __WLAN_80211_H__

#include "common.h"

typedef struct {
	uint8_t version;	
    uint8_t pad;					
	uint16_t length;			
	uint32_t present;		
    uint16_t flags;
    uint16_t channel_frequency;
    uint16_t channel_flags;
    uint8_t antenna_signal;
} ieee80211_radiotap_header;


typedef struct 
{
    uint8_t frame_type;
    uint16_t frame_control;
    uint16_t duration;
    uint8_t daddr[6];
    uint8_t wifiaddr[6];
    uint8_t saddr[6];
    uint8_t bss_id[9];
} ieee80211_radiotap_frame;
//32

int ieee80211RadiotapHeader(uint8_t *buffer, int size);

#endif