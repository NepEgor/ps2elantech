#ifndef PS2ELANTECH_PS2_H
#define PS2ELANTECH_PS2_H

#include "const_queue.h"

#include <Arduino.h>

class PS2
{
    private:
    
    enum State: uint8_t
    {
        READ,
        WRITE_START,
        WRITE,
    };

    uint32_t clockPin;
    uint32_t dataPin;

    volatile State state;

    ConstQueue queue;

    // interupt variables
    volatile uint8_t raw;
    volatile uint8_t shift;
    volatile uint8_t parity;

    // interupt handlers
    void int_read();
    void int_write();

    public:

    PS2() {}

    void initialize(uint32_t clockPin, uint32_t dataPin);
    
    void test();

    uint8_t readByte(uint8_t &data);

    uint8_t readByteAsync(uint8_t &data);

    uint8_t queueSize() {return queue.Size();}

    uint8_t writeByte(uint8_t data);

    // write command byte
    // command format 0xARCC
    // A - number of args
    // R - number of returns
    // CC - command
    // command args and returns in params array
    uint8_t command(uint16_t command, uint8_t *param = NULL);
    
    // sliced_command() sends an extended PS/2 command to the mouse
    // using sliced syntax, understood by advanced devices, such as Logitech
    // or Synaptics touchpads. The command is encoded as:
    // 0xE6 0xE8 rr 0xE8 ss 0xE8 tt 0xE8 uu where (rr*64)+(ss*16)+(tt*4)+uu
    // is the command.
    uint8_t sliced_command(uint8_t command);

    void purgeQueue();

    void int_on_clock();
};

#endif // PS2_H
