#ifndef PS2_H
#define PS2_H

#include "queue.h"

#include <Arduino.h>

enum State: uint8_t
{
    IDLE,
    READ,
    WRITE_START,
    WRITE,
};

class PS2
{
    public:
    
    int clockPin;
    int dataPin;

    State state;

    Queue queue;

    volatile uint16_t raw;
    volatile uint8_t shift;
    volatile uint8_t parity;

    volatile uint32_t start;
    volatile uint16_t interval;

    void int_read();
    void int_write();

    public:

    PS2(int clockPin, int dataPin);

    void initialize();
    
    State getState();
    void setIdle();

    uint8_t readByte();
    void writeByte(uint8_t data);

    void command(uint16_t command, uint8_t *param);

    void int_on_clock();
};

#endif // PS2_H
