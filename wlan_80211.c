#include "wlan_80211.h"
#include <stdio.h>
#include "bitstream.h"


int parse_header(uint8_t *buffer, int size)
{
    bs_t* bs = bs_new(buffer, size);
    bs_read_u8(bs);
    bs_read_u8(bs);
    uint32_t length  = bs_read_u8(bs);
    bs_free(bs);
    return length;
}

int parse_ieee80211header(uint8_t *buffer, int size)
{
    bs_t* bs = bs_new(buffer, size);

    uint32_t frame_type = bs_read_u8(bs);
    bs_read_u(bs, 24); 

    uint8_t daddr[6];
    uint8_t wifiaddr[6];
    uint8_t saddr[6];

    daddr[0] = bs_read_u8(bs);
    daddr[1] = bs_read_u8(bs);
    daddr[2] = bs_read_u8(bs);
    daddr[3] = bs_read_u8(bs);
    daddr[4] = bs_read_u8(bs);
    daddr[5] = bs_read_u8(bs);
    
    wifiaddr[0] = bs_read_u8(bs);
    wifiaddr[1] = bs_read_u8(bs);
    wifiaddr[2] = bs_read_u8(bs);
    wifiaddr[3] = bs_read_u8(bs);
    wifiaddr[4] = bs_read_u8(bs);
    wifiaddr[5] = bs_read_u8(bs);

    saddr[0] = bs_read_u8(bs);
    saddr[1] = bs_read_u8(bs);
    saddr[2] = bs_read_u8(bs);
    saddr[3] = bs_read_u8(bs);
    saddr[4] = bs_read_u8(bs);
    saddr[5] = bs_read_u8(bs);


    if (saddr[0] == 0x50 && saddr[1] == 0x7B && saddr[2] == 0x91 && frame_type == 0x08) {
        printf("frame_type  %u, ", frame_type);
        printf("dst %02X:%02X:%02X:%02X:%02X:%02X, ", 
            daddr[0], daddr[1], daddr[2], daddr[3], daddr[4], daddr[5]); 
        printf("wifiaddr %02X:%02X:%02X:%02X:%02X:%02X, ", 
                wifiaddr[0], wifiaddr[1], wifiaddr[2], wifiaddr[3], wifiaddr[4], wifiaddr[5]); 
        printf("src %02X:%02X:%02X:%02X:%02X:%02X, ", 
                saddr[0], saddr[1], saddr[2], saddr[3], saddr[4], saddr[5]);

        printf("size %03X\n",  size);
    }

    bs_free(bs);

    return 32;        
}

int parse_ieee80211_radiotap_header(uint8_t *buffer, int size)
{
    int len = parse_header(buffer, size);
    len += parse_ieee80211header(buffer + len, size);
    len += 36;
    //printf("size %d, %d\n", size -len, size);
    return 0;
}
