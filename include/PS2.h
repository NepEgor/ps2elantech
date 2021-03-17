#ifndef PS2_H
#define PS2_H

#include "debug.h"

#include "queue.h"

#include <Arduino.h>

enum State: uint8
{
    IDLE,
    WRITE_START,
    WRITE,
    WRITE_FINISH,
    READ,
    READ_FINISH,
};

class PS2
{
    public:
    
    uint8 clockPin;
    uint8 dataPin;

    State state;

    Queue queue;

    // interupt variables
    volatile uint8 raw;
    volatile uint8 shift;
    volatile uint8 parity;

    // delay for holding clock before sending byte
    uint32 start;
    uint16 interval;

    // command variables
    uint8 left_bytes;
    uint8 send_bytes;
    uint8 recv_bytes;
    uint8 buf;
    bool handleACK;

    // sliced_command variables
    int8 sliced_shift;
    uint8 command_part;

    // interupt handlers
    void int_read();
    void int_write();

    public:

    PS2() {}

    void initialize(uint8 clockPin, uint8 dataPin);
    
    State getState();
    void setIdle();
    uint8 getIdle();

    void startReading();
    uint8 readByte(uint8 &data);
    uint8 writeByte(uint8 data);

    // write command byte
    // command format 0xARCC
    // A - number of args
    // R - number of returns
    // CC - command
    // command args and returns in params array
    uint8 command(uint16 command, uint8 *param, bool wait = false);
    
    // sliced_command() sends an extended PS/2 command to the mouse
    // using sliced syntax, understood by advanced devices, such as Logitech
    // or Synaptics touchpads. The command is encoded as:
    // 0xE6 0xE8 rr 0xE8 ss 0xE8 tt 0xE8 uu where (rr*64)+(ss*16)+(tt*4)+uu
    // is the command.
    uint8 sliced_command(uint16 command, bool wait = false);

    void int_on_clock();
};

#endif // PS2_H
