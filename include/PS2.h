#ifndef PS2_H
#define PS2_H

#include "debug.h"

#include "queue.h"

#include <Arduino.h>

enum State: uint8_t
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
    
    uint32_t clockPin;
    uint32_t dataPin;

    State state;

    Queue queue;

    // interupt variables
    volatile uint8_t raw;
    volatile uint8_t shift;
    volatile uint8_t parity;

    // delay for holding clock before sending byte
    uint32_t start;
    uint16_t interval;

    // command variables
    uint8_t left_bytes;
    uint8_t send_bytes;
    uint8_t recv_bytes;
    uint8_t buf;
    bool handleACK;

    // sliced_command variables
    int8_t sliced_shift;
    uint8_t command_part;

    // interupt handlers
    void int_read();
    void int_write();

    public:

    PS2() {}

    void initialize(uint32_t clockPin, uint32_t dataPin);
    
    State getState();
    void setIdle();
    uint8_t getIdle();

    uint8_t readByte(uint8_t &data);
    uint8_t writeByte(uint8_t data);

    // write command byte
    // command format 0xARCC
    // A - number of args
    // R - number of returns
    // CC - command
    // command args and returns in params array
    uint8_t command(uint16_t command, uint8_t *param = NULL, bool wait = false);
    
    // sliced_command() sends an extended PS/2 command to the mouse
    // using sliced syntax, understood by advanced devices, such as Logitech
    // or Synaptics touchpads. The command is encoded as:
    // 0xE6 0xE8 rr 0xE8 ss 0xE8 tt 0xE8 uu where (rr*64)+(ss*16)+(tt*4)+uu
    // is the command.
    uint8_t sliced_command(uint16_t command, bool wait = false);

    void int_on_clock();
};

#endif // PS2_H
