#ifndef PS2_H
#define PS2_H

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
    
    int clockPin;
    int dataPin;

    State state;

    Queue queue;

    volatile uint8_t raw;
    volatile uint8_t shift;
    volatile uint8_t parity;

    volatile uint32_t start;
    volatile uint16_t interval;

    volatile uint8_t left_bytes;
    volatile uint8_t send_bytes;
    volatile uint8_t recv_bytes;

    void int_read();
    void int_write();

    public:

    PS2(int clockPin, int dataPin);

    void initialize();
    
    State getState();
    void setIdle();
    uint8_t getIdle();

    void startReading();
    uint8_t readByte(uint8_t &data);
    void writeByte(uint8_t data);

    void commandWait(uint16_t command, uint8_t *param);
    uint8_t command(uint16_t command, uint8_t *param);

    void int_on_clock();
};

#endif // PS2_H
