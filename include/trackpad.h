#ifndef TRACKPAD_H
#define TRACKPAD_H

#include "debug.h"

#include "PS2.h"

// Elantech trackpad
class TrackPad
{
    public:

    PS2 ps2;

    uint8  hw_version;
    uint32 fw_version;
    uint8  ic_version;

    uint8 command_state;

    void elantech_detect();

    public:

    TrackPad();

    void initialize(uint8 clockPin, uint8 dataPin);

    uint8 elantech_command(uint8 command, uint8 *param, bool wait = false);

    void inline int_on_clock(){
        ps2.int_on_clock();
    }
};

#endif // TRACKPAD_H