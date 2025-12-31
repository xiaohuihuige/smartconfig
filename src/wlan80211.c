#include "wlan80211.h"
#include <stdio.h>
#include "bitstream.h"

int ieee80211RadiotapHeader(ieee80211_radiotap *radiotap, uint8_t *buffer, int size)
{
    ST_CHECK_POINTER(radiotap);
    ST_CHECK_POINTER(buffer);

    ieee80211_radiotap_header header;
    ieee80211_radiotap_frame frame;
   
    bs_t* bs = bs_new(buffer, size);
    if (!bs) 
        return SMART_FAIL;
        
    header.version = bs_read_u8(bs);
    header.pad     = bs_read_u8(bs);
    header.length  = bs_read_u8(bs);

    bs_read_u8n(bs, header.length - bs_pos(bs));

    radiotap->frame_type = frame.frame_type = bs_read_u8(bs);
    if (frame.frame_type != 0x08) {
        bs_free(bs);
        return SMART_FAIL;
    }

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
    radiotap->saddr[0] = frame.saddr[0] = bs_read_u8(bs);
    radiotap->saddr[1] = frame.saddr[1] = bs_read_u8(bs);
    radiotap->saddr[2] = frame.saddr[2] = bs_read_u8(bs);
    radiotap->saddr[3] = frame.saddr[3] = bs_read_u8(bs);
    radiotap->saddr[4] = frame.saddr[4] = bs_read_u8(bs);
    radiotap->saddr[5] = frame.saddr[5] = bs_read_u8(bs);
    bs_free(bs);
    return size-header.length-76;
}


