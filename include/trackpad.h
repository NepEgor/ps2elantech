#ifndef TRACKPAD_H
#define TRACKPAD_H

#include "debug.h"

#include "PS2.h"

// Elantech trackpad
class TrackPad
{
    private:

    PS2 ps2;

    uint8  hw_version;
    uint32 fw_version;
    uint8  ic_version;

    uint8 capabilities[3];

    uint32 x_res;
    uint32 y_res;
    uint32 bus;

    uint32 x_max;
    uint32 y_max;

    uint8 x_traces;
    uint8 y_traces;
    uint8 width;

    uint8 reg_07;
    uint8 reg_10;
    uint8 reg_11;
    uint8 reg_20;
    uint8 reg_21;
    uint8 reg_22;
    uint8 reg_23;
    uint8 reg_24;
    uint8 reg_25;
    uint8 reg_26;

    uint8 write_reg_state;

    uint8 timeout_tries;
    uint8 timeout_state;
    uint32 timeout_start;

    uint8 command_state;

    void elantech_detect();
    void elantech_query_info();
    void elantech_setup_ps2();

    public:

    TrackPad();

    void initialize(uint8 clockPin, uint8 dataPin);

    uint8 elantech_command(uint8 command, uint8 *param, bool wait = false);
    uint8 elantech_write_reg(uint8 reg, uint8 val, bool wait = false);
    
    uint8 ps2_command_timeout(uint16 command, uint8 *param = NULL, bool wait = false);

    void inline int_on_clock(){
        ps2.int_on_clock();
    }
};

#endif // TRACKPAD_H