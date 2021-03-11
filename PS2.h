#ifndef PS2_H
#define PS2_H

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
    private:
    
    uint8 clockPin;
    uint8 dataPin;

    State state;

    Queue queue;

    volatile uint8 raw;
    volatile uint8 shift;
    volatile uint8 parity;

    volatile uint32 start;
    volatile uint16 interval;

    volatile uint8 left_bytes;
    volatile uint8 send_bytes;
    volatile uint8 recv_bytes;

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
    void writeByte(uint8 data);

    void commandWait(uint16 command, uint8 *param);
    uint8 command(uint16 command, uint8 *param);

    void int_on_clock();
};

#endif // PS2_H
