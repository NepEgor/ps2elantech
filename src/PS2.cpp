#include "PS2.h"

#include "ps2defines.h"

void high(int pin) {
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

void low(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void PS2::initialize(uint32_t clockPin, uint32_t dataPin) {
    this->clockPin = clockPin;
    this->dataPin = dataPin;

    raw = 0;
    shift = 0;
    parity = 1;

    low(clockPin);
    low(dataPin);

    state = READ;
}

void PS2::test() {

}

uint8_t PS2::sliced_command(uint16_t command) {
    
    return 1;
}

uint8_t PS2::readPacket(uint8_t *packet, uint8_t size) {
    
    return 1;
}

uint8_t PS2::command(uint16_t command, uint8_t *param) {
    
    return 1;
}

uint8_t PS2::readByte(uint8_t &data) {
    
    return 1;
}

uint8_t PS2::writeByte(uint8_t data) {
    
    return 1;
}

void PS2::int_on_clock() {

    switch(state) {
        case WRITE:
            int_write();
            break;

        default:
            int_read();
            break;
    }

}

void PS2::int_read() {
    uint8_t bit = digitalRead(dataPin);

    // 0 - start bit
    // 1-8 - data
    // 9 - parity
    // 10 - stop
    switch(shift) {
        
        case 0: // first bit must be 0
            if(bit == 1)
                goto reset_frame;
            ++shift;
            return;
        
        case 9: // parity
            if (__builtin_parity(raw) == bit)
                goto reset_frame;
            ++shift;
            return;
        
        case 10: // stop
            queue.push(raw);
            // state = READ_FINISH;
            goto reset_frame;

        default:
            break;
    }

    parity ^= bit;
    raw |= bit << (shift - 1);
    ++shift;
    return;

    reset_frame:
        raw = 0;
        shift = 0;
        parity = 1;
}

void PS2::int_write() {
    uint8_t bit = 0;

    // start bit sent in writeByte()
    // 0-7 - data
    // 8 - parity
    // 9 - stop
    // 10 - read ack
    switch(shift) {
        case 0 ... 7:
            bit = (raw >> shift) & 1;
            parity ^= bit;
            digitalWrite(dataPin, bit);
            ++shift;
            break;

        case 8:
            digitalWrite(dataPin, parity);
            ++shift;
            break;

        case 9:
            high(dataPin);
            ++shift;
            break;

        case 10:
            bit = digitalRead(dataPin);
            state = READ; // read ACK
            // reset
            raw = 0;
            shift = 0;
            parity = 1;
            break;

        default:
            break;
    }
}