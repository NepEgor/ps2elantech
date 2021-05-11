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

    bool sanity_check;

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

    //Serial.print("Sanity: ");
    //Serial.println(sanity_check);

    if (!sanity_check) {
        packet_type = PACKET_UNKNOWN;
    }
    else {
        //Serial.print("Type Raw: ");
        //Serial.println(packet_type);

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

        //Serial.print("Type Enum: ");
        //Serial.println(packet_type);
    }

	return packet_type;
}

void TrackPad::process_packet_status_v4() {
    Serial.println("Status");

}

void TrackPad::process_packet_head_v4() {
    Serial.println("Head");

}

void TrackPad::process_packet_motion_v4() {
    Serial.println("Motion");

}

uint8_t TrackPad::poll() {
    if(!ps2.command(PS2_CMD_POLL, packet)) {
        elantech_packet_check_v4();
        switch(packet_type) {
            case PACKET_TRACKPOINT:
            case PACKET_UNKNOWN:
                Serial.println("Bad Data");
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

        Serial.println();
    }

    return 0;
}