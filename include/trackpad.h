#ifndef PS2ELANTECH_TRACKPAD_H
#define PS2ELANTECH_TRACKPAD_H

#include "PS2.h"

void printParam(uint8_t *param, uint8_t len = 3);

struct FingerPosition
{
    int32_t x;
    int32_t y;
    int32_t dx;
    int32_t dy;
};

enum TouchEventType : uint8_t
{
    TET_DOWN,
    TET_MOVE,
    TET_UP,
};

struct TouchEvent
{
    TouchEventType type;
    uint8_t finger_id;
    FingerPosition fp;
};

// Elantech trackpad
class TrackPad
{
    private:

    PS2 ps2;

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

    /* unused
    uint8_t reg_11;
    uint8_t reg_20;
    uint8_t reg_21;
    uint8_t reg_22;
    uint8_t reg_23;
    uint8_t reg_24;
    uint8_t reg_25;
    uint8_t reg_26;
    */

    uint8_t write_reg_state;

    /* unused
    uint8_t timeout_tries;
    uint8_t timeout_state;
    uint32_t timeout_start;

    uint8_t command_state;
    */

    static const uint8_t packet_size = 6;
    uint8_t packet_type;
    uint8_t packet[packet_size];

    public:
    static const uint8_t fingers_num = 5;
    private:
    uint8_t touching_prev;
    uint8_t touching;
    FingerPosition fingers[fingers_num];

    void elantech_detect();
    void elantech_query_info();
    void elantech_setup_ps2();

    //void resync();

    public:

    TrackPad();

    void initialize(uint8_t clockPin, uint8_t dataPin);

    uint8_t elantech_command(uint8_t command, uint8_t *param, bool wait = false);
    uint8_t elantech_write_reg(uint8_t reg, uint8_t val, bool wait = false);
    
    //uint8_t ps2_command_timeout(uint16_t command, uint8_t *param = NULL, bool wait = false);

    int8_t poll(TouchEvent* tevent, uint8_t &size);

    int32_t getMaxX() {return x_max;}
    int32_t getMaxY() {return y_max;}

    uint8_t elantech_packet_check_v4();
    void process_packet_status_v4(TouchEvent* tevent, uint8_t &size);
    void process_packet_head_v4(TouchEvent* tevent, uint8_t &size);
    void process_packet_motion_v4(TouchEvent* tevent, uint8_t &size);

    void inline int_on_clock(){
        ps2.int_on_clock();
    }
};

#endif // TRACKPAD_H