#ifndef TRACKPAD_H
#define TRACKPAD_H

#include "debug.h"

#include "PS2.h"

class TrackPad
{
    public:

    PS2 ps2;

    void elantech_detect();

    public:

    TrackPad() {}

    void initialize(uint8 clockPin, uint8 dataPin);

    void inline int_on_clock(){
        ps2.int_on_clock();
    }
};

#endif // TRACKPAD_H