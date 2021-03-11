#include "trackpad.h"

void TrackPad::initialize(uint8 clockPin, uint8 dataPin) {
    ps2.initialize(clockPin, dataPin);

    uint8 param[3] = {0xFF, 0xFF, 0xFF};

    ps2.commandWait(0x02FF, param);

    for(uint8 i = 0; i < 2; ++i) {
        CSerial.println(param[i], BIN);
        param[i] = 0xFF;
    }

    ps2.commandWait(0x01F2, param);

    for(uint8 i = 0; i < 1; ++i) {
        CSerial.println(param[i], BIN);
    }
}