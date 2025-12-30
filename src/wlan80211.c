#include "wlan80211.h"
#include <stdio.h>
#include "bitstream.h"

int ieee80211RadiotapHeader(uint8_t *buffer, int size)
{
    ieee80211_radiotap_header header;
    ieee80211_radiotap_frame frame;
   
    bs_t* bs = bs_new(buffer, size);
    header.version = bs_read_u8(bs);
    header.pad     = bs_read_u8(bs);
    header.length  = bs_read_u8(bs);


    bs_read_u8n(bs, header.length - bs_pos(bs));

    frame.frame_type = bs_read_u8(bs);
    bs_read_u8(bs);
    frame.duration = bs_read_u(bs, 16);
    frame.daddr[0] = bs_read_u8(bs);
    frame.daddr[1] = bs_read_u8(bs);
    frame.daddr[2] = bs_read_u8(bs);
    frame.daddr[3] = bs_read_u8(bs);
    frame.daddr[4] = bs_read_u8(bs);
    frame.daddr[5] = bs_read_u8(bs);
    frame.wifiaddr[0] = bs_read_u8(bs);
    frame.wifiaddr[1] = bs_read_u8(bs);
    frame.wifiaddr[2] = bs_read_u8(bs);
    frame.wifiaddr[3] = bs_read_u8(bs);
    frame.wifiaddr[4] = bs_read_u8(bs);
    frame.wifiaddr[5] = bs_read_u8(bs);
    frame.saddr[0] = bs_read_u8(bs);
    frame.saddr[1] = bs_read_u8(bs);
    frame.saddr[2] = bs_read_u8(bs);
    frame.saddr[3] = bs_read_u8(bs);
    frame.saddr[4] = bs_read_u8(bs);
    frame.saddr[5] = bs_read_u8(bs);
    bs_free(bs);

    if (frame.frame_type == 0x08) {
        // LOG("dest addr: %02X:%02X:%02X:%02X:%02X:%02X,"
        //     "transmitter: %02X:%02X:%02X:%02X:%02X:%02X,"
        //     "source addr: %02X:%02X:%02X:%02X:%02X:%02X, size %d",  
        //     frame.daddr[0], frame.daddr[1], frame.daddr[2], frame.daddr[3], frame.daddr[4], frame.daddr[5],
        //     frame.wifiaddr[0], frame.wifiaddr[1], frame.wifiaddr[2], frame.wifiaddr[3], frame.wifiaddr[4], frame.wifiaddr[5],
        //     frame.saddr[0], frame.saddr[1], frame.saddr[2], frame.saddr[3], frame.saddr[4], frame.saddr[5],
        //     size-header.length-32-76); 
        //printfChar(buffer, size);
        return size-header.length-76;
    }
    return 0;
}


