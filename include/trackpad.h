#ifndef TRACKPAD_H
#define TRACKPAD_H

#include "debug.h"

#include "PS2.h"

void printParam(uint8_t *param, uint8_t len = 3);

struct Position
{
    int32_t x;
    int32_t y;
    int32_t dx;
    int32_t dy;
};

// Elantech trackpad
class TrackPad
{
    private:

    PS2 ps2;

    uint8_t id;

    uint8_t  hw_version;
    uint32_t fw_version;
    uint8_t  ic_version;
    bool crc_enabled;

    uint8_t capabilities[3];
    uint8_t samples[3];

    uint32_t x_res;
    uint32_t y_res;
    uint32_t bus;

    int32_t x_max;
    int32_t y_max;

    uint8_t x_traces;
    uint8_t y_traces;
    uint8_t width;

    uint8_t reg_07;
    uint8_t reg_10;
    uint8_t reg_11;
    uint8_t reg_20;
    uint8_t reg_21;
    uint8_t reg_22;
    uint8_t reg_23;
    uint8_t reg_24;
    uint8_t reg_25;
    uint8_t reg_26;

    uint8_t write_reg_state;

    uint8_t timeout_tries;
    uint8_t timeout_state;
    uint32_t timeout_start;

    uint8_t command_state;

    static const uint8_t packet_size = 6;
    uint8_t packet_type;
    uint8_t packet[packet_size];

    uint8_t touching_prev;
    uint8_t touching;
    Position fingers[5];

    uint32_t packet_i;

    void elantech_detect();
    void elantech_query_info();
    void elantech_setup_ps2();

    void resync();

    public:

    TrackPad(uint8_t id);

    void initialize(uint8_t clockPin, uint8_t dataPin);

    uint8_t elantech_command(uint8_t command, uint8_t *param, bool wait = false);
    uint8_t elantech_write_reg(uint8_t reg, uint8_t val, bool wait = false);
    
    //uint8_t ps2_command_timeout(uint16_t command, uint8_t *param = NULL, bool wait = false);

    uint8_t poll();

    uint8_t elantech_packet_check_v4();
    void process_packet_status_v4();
    void process_packet_head_v4();
    void process_packet_motion_v4();

    void inline int_on_clock(){
        ps2.int_on_clock();
    }
};

#endif // TRACKPAD_H