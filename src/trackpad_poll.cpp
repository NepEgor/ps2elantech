#include "trackpad.h"

#include "ps2defines.h"

//#include <Mouse.h>

enum PacketType: uint8_t
{
    PACKET_UNKNOWN	    = 0x01,
    PACKET_DEBOUNCE	    = 0x02,
    PACKET_V3_HEAD	    = 0x03,
    PACKET_V3_TAIL	    = 0x04,
    PACKET_V4_HEAD	    = 0x05,
    PACKET_V4_MOTION	= 0x06,
    PACKET_V4_STATUS	= 0x07,
    PACKET_TRACKPOINT	= 0x08,
};

/*
//Function to get gcd of a and b
uint8_t gcd(uint8_t a, uint8_t b)
{
    if (b == 0)
        return a;
 
    else
        return gcd(b, a % b);
}
 
//Function to left rotate arr[] of siz n by d
void leftRotate(uint8_t* arr, uint8_t d, uint8_t n)
{
    // To handle if d >= n
    d = d % n;
    uint8_t g_c_d = gcd(d, n);
    for (uint8_t i = 0; i < g_c_d; i++) {
        // move i-th values of blocks
        uint8_t temp = arr[i];
        uint8_t j = i;
 
        while (1) {
            uint8_t k = j + d;
            if (k >= n)
                k = k - n;
 
            if (k == i)
                break;
 
            arr[j] = arr[k];
            j = k;
        }
        arr[j] = temp;
    }
}

void TrackPad::resync() {
    Serial.print("Resyncing ");
    uint8_t i = 0;
    //for (uint8_t i = 1; i < packet_size; ++i) {
    while(1) {
        ps2.readByte(packet[i % packet_size]);
        ++i;

        if((packet[(0 + i) % packet_size] & 0x08) == 0x00 &&
		   (packet[(3 + i) % packet_size] & 0x1c) == 0x10)
        {
            leftRotate(packet, i, packet_size);
            Serial.println(i);
            return;
        }
    }

    Serial.println("Uanble to resync!");
}
*/

uint8_t TrackPad::elantech_packet_check_v4() {
    packet_type = packet[3] & 0x03;

    bool sanity_check = true;
    /*
    if(crc_enabled) {
		sanity_check = ((packet[3] & 0x08) == 0x00);
    }
	else
    if(ic_version == 7 && samples[1] == 0x2A) {
		sanity_check = ((packet[3] & 0x1c) == 0x10);
    }
	else {*/
		sanity_check = ((packet[0] & 0x08) == 0x00 &&
				        (packet[3] & 0x1c) == 0x10);
    //}
    
    if (!sanity_check) {
        packet_type = PACKET_UNKNOWN;
        //resync();
        //packet_type = packet[3] & 0x03;
    }
    else {
        switch (packet_type) {
            case 0:
                packet_type = PACKET_V4_STATUS;
                break;

            case 1:
                packet_type = PACKET_V4_HEAD;
                break;

            case 2:
                packet_type = PACKET_V4_MOTION;
                break;

            default:
                packet_type = PACKET_UNKNOWN;
                break;
        }
    }

	return packet_type;
}

void TrackPad::process_packet_status_v4(TouchEvent* tevent, uint8_t &size) {
    //Serial.println("Status");

    touching_prev = touching;
    touching = packet[1] & 0x1f;

    uint8_t dif01 = (touching ^ touching_prev) & ~touching_prev; // sets changes from 0 to 1
    uint8_t dif10 = (touching ^ touching_prev) & ~touching;      // sets changes from 1 to 0

    //Serial.printf("%u S %X %X %X %X\n", id, touching, touching_prev, dif01, dif10);

    if (dif01) {
        for (uint8_t i = 0; i < fingers_num; ++i) {

            if(dif01 & 1) {
                fingers[i].dx = 0;
                fingers[i].dy = 0;
            }

            dif01 >>= 1;
        }
    }

    if (dif10) {
        for (uint8_t i = 0; i < fingers_num; ++i) {

            if(dif10 & 1) {
                tevent[size].finger_id = i;
                tevent[size].type = TET_UP;
                ++size;
            }

            dif10 >>= 1;
        }
    }
}

void TrackPad::process_packet_head_v4(TouchEvent* tevent, uint8_t &size) {
    //Serial.println("Head");

    int8_t fid = ((packet[3] & 0xe0) >> 5) - 1;
    if(fid < 0) return;
    
    size = 1;

    uint8_t pres, traces;

    int32_t x = ((packet[1] & 0x0f) << 8) | packet[2];
    int32_t y = ((packet[4] & 0x0f) << 8) | packet[5];

    // if finger just touched down
    if (((touching ^ touching_prev) & (1 << fid))) {
        fingers[fid].dx = 0;
        fingers[fid].dy = 0;

        touching_prev ^= touching;

        tevent[0].type = TET_DOWN;
    }
    // if finger has been touching
    else {
        fingers[fid].dx = fingers[fid].x - x;
        fingers[fid].dy = fingers[fid].y - y;

        tevent[0].type = TET_MOVE;
    }

    fingers[fid].x = x;
	fingers[fid].y = y;

	pres = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
	traces = (packet[0] & 0xf0) >> 4;

    tevent[0].finger_id = fid;
    tevent[0].fp.x  = x;
    tevent[0].fp.y  = y;
    tevent[0].fp.dx = fingers[fid].dx;
    tevent[0].fp.dy = fingers[fid].dy;

    //Serial.printf("%u H f %i  x %i  y %i\n", id, fid, fingers[fid].x, fingers[fid].y);
    /*
    Serial.print("f ");
    Serial.print(id);
    Serial.print("  x ");
    Serial.print(fingers[id].x);
    Serial.print("  y ");
    Serial.println(fingers[id].y);
    */
    //Serial.print("  p ");
    //Serial.print(pres);
    //Serial.print("  t ");
    //Serial.println(traces * width);
}

void TrackPad::process_packet_motion_v4(TouchEvent* tevent, uint8_t &size) {
    //Serial.println("Motion");

    int16_t fid = ((packet[0] & 0xe0) >> 5) - 1;
    if (fid < 0) return;

    int16_t sid = ((packet[3] & 0xe0) >> 5) - 1;
    int16_t weight = (packet[0] & 0x10) ? 5 : 1;

    int32_t delta_x1 = (int8_t)packet[1];
	int32_t delta_y1 = (int8_t)packet[2];
	int32_t delta_x2 = (int8_t)packet[4];
	int32_t delta_y2 = (int8_t)packet[5];
    
    fingers[fid].dx = delta_x1 * weight;
    fingers[fid].dy = delta_y1 * weight;

    fingers[fid].x += fingers[fid].dx;
    fingers[fid].y += fingers[fid].dy;

    size = 1;
    tevent[0].finger_id = fid;
    tevent[0].type = TET_MOVE;
    tevent[0].fp.x  = fingers[fid].x;
    tevent[0].fp.y  = fingers[fid].y;
    tevent[0].fp.dx = fingers[fid].dx;
    tevent[0].fp.dy = fingers[fid].dy;

    //Serial.printf("%u M f %i  x %i  y %i", id, fid, fingers[fid].x, fingers[fid].y);
    /*
    Serial.print("f1 ");
    Serial.print(id);
    Serial.print("  x ");
    Serial.print(fingers[id].x);
    Serial.print("  y ");
    Serial.println(fingers[id].y);
    */

    if(sid >= 0) {
        fingers[sid].dx = delta_x2 * weight;
        fingers[sid].dy = delta_y2 * weight;

        fingers[sid].x += fingers[sid].dx;
        fingers[sid].y += fingers[sid].dy;
    
        size = 2;
        tevent[1].finger_id = sid;
        tevent[1].type = TET_MOVE;
        tevent[1].fp.x  = fingers[sid].x;
        tevent[1].fp.y  = fingers[sid].y;
        tevent[1].fp.dx = fingers[sid].dx;
        tevent[1].fp.dy = fingers[sid].dy;
        //Serial.printf("  f %i  x %i  y %i\n", sid, fingers[sid].x, fingers[sid].y);
        /*
        Serial.print("f2 ");
        Serial.print(sid);
        Serial.print("  x2 ");
        Serial.print(fingers[sid].x);
        Serial.print("  y2 ");
        Serial.println(fingers[sid].y);*/
    }
    else{
        //Serial.println();
    }
}

int8_t TrackPad::poll(TouchEvent* tevent, uint8_t &size) {
    if (ps2.queueSize() >= packet_size) {
        for (uint8_t i = 0; i < packet_size; ++i) {
            ps2.readByteAsync(packet[i]);
        }
        //printParam(packet, packet_size);

        size = 0;

        elantech_packet_check_v4();
        switch(packet_type) {
            case PACKET_V4_STATUS:
                process_packet_status_v4(tevent, size);
                break;

            case PACKET_V4_HEAD:
                process_packet_head_v4(tevent, size);
                break;

            case PACKET_V4_MOTION:
                process_packet_motion_v4(tevent, size);
                break;
            
            case PACKET_UNKNOWN:
                //Serial.printf("Bad Data; Queue: %u\n", ps2.queueSize());
                return -2;
                break;
            
            case PACKET_TRACKPOINT:
                //Serial.println("Trackpoint");
                return -2;
                break;

            // TODO V3

            default:
                return -3;
                break;
        }

        //Serial.println();
        return size;
    }
    
    return -1;
}