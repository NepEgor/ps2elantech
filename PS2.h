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
    private:
    
    uint8 clockPin;
    uint8 dataPin;

    State state;

    Queue queue;

    volatile uint8 raw;
    volatile uint8 shift;
    volatile uint8 parity;

    uint32 start;
    uint16 interval;

    uint8 left_bytes;
    uint8 send_bytes;
    uint8 recv_bytes;

    int8 sliced_shift;
    uint8 command_part;

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
    uint8 sliced_command(uint16 command);

    void int_on_clock();
};

#endif // PS2_H
