#include "trackpad.h"

#include "ps2defines.h"

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

uint8_t TrackPad::elantech_packet_check_v4() {
    packet_type = packet[3] & 0x03;

    bool sanity_check = true;
    
    if(crc_enabled) {
		sanity_check = ((packet[3] & 0x08) == 0x00);
    }
	else
    if(ic_version == 7 && samples[1] == 0x2A) {
		sanity_check = ((packet[3] & 0x1c) == 0x10);
    }
	else {
		sanity_check = ((packet[0] & 0x08) == 0x00 &&
				        (packet[3] & 0x1c) == 0x10);
    }
    
    if (!sanity_check) {
        packet_type = PACKET_UNKNOWN;
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

void TrackPad::process_packet_status_v4() {
    //Serial.println("Status");

    uint8_t fingers = packet[1] & 0x1f;
    //Serial.println(fingers, BIN);
}

void TrackPad::process_packet_head_v4() {
    //Serial.println("Head");

    int8_t id = ((packet[3] & 0xe0) >> 5) - 1;
    if(id < 0) return;
    
    uint8_t pres, traces;

    fingers[id].x = ((packet[1] & 0x0f) << 8) | packet[2];
	fingers[id].y = y_max - (((packet[4] & 0x0f) << 8) | packet[5]);

	pres = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
	traces = (packet[0] & 0xf0) >> 4;

    //Serial.print("f ");
    //Serial.print(id);
    Serial.print("  x ");
    Serial.print(fingers[id].x);
    Serial.print("  y ");
    Serial.println(fingers[id].y);
    //Serial.print("  p ");
    //Serial.print(pres);
    //Serial.print("  t ");
    //Serial.println(traces * width);
}

void TrackPad::process_packet_motion_v4() {
    //Serial.println("Motion");

    int16_t id = ((packet[0] & 0xe0) >> 5) - 1;
    if (id < 0) return;

    int16_t sid = ((packet[3] & 0xe0) >> 5) - 1;
    int16_t weight = (packet[0] & 0x10) ? 5 : 1;

    int16_t delta_x1 = (int8_t)packet[1];
	int16_t delta_y1 = (int8_t)packet[2];
	int16_t delta_x2 = (int8_t)packet[4];
	int16_t delta_y2 = (int8_t)packet[5];

    fingers[id].x += delta_x1 * weight;
    fingers[id].y += delta_y1 * weight;

    //Serial.print("f1 ");
    //Serial.print(id);
    Serial.print("  x ");
    Serial.print(fingers[id].x);
    Serial.print("  y ");
    Serial.println(fingers[id].y);

    if(sid >= 0) {
        fingers[sid].x += delta_x2 * weight;
        fingers[sid].y += delta_y2 * weight;
        /*
        Serial.print("f2 ");
        Serial.print(sid);
        Serial.print("  x2 ");
        Serial.print(fingers[sid].x);
        Serial.print("  y2 ");
        Serial.println(fingers[sid].y);*/
    }
}

uint8_t TrackPad::poll() {
    //if(!ps2.readPacket(packet, packet_size)) {
    if(!ps2.readByte(packet[packet_i])) {
        //ps2.setIdle();
        
        packet_i += 1;
        if (packet_i < packet_size) return 1;
        
        packet_i = 0;
        //Serial.println(packet_i);

        //printParam(packet, packet_size);

        elantech_packet_check_v4();
        switch(packet_type) {
            case PACKET_UNKNOWN:
                Serial.printf("Bad Data; Queue: %u\n", ps2.queueSize());
                break;
            
            case PACKET_TRACKPOINT:
                //Serial.println("Trackpoint");
                break;

            case PACKET_V4_STATUS:
                process_packet_status_v4();
                break;

            case PACKET_V4_HEAD:
                process_packet_head_v4();
                break;

            case PACKET_V4_MOTION:
                process_packet_motion_v4();
                break;
            
            // TODO V3

            default:
                break;
        }

        //Serial.println();
    }

    return 0;
}