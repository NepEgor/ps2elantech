#include "trackpad.h"

#include "ps2defines.h"

uint8_t TrackPad::poll() {
    if(!ps2.command(PS2_CMD_POLL, packet)){
        printParam(packet, packet_size);
        Serial.println();
    }

    return 0;
}