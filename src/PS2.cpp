#include "PS2.h"

#include "ps2defines.h"

void PS2::initialize(uint32_t clockPin, uint32_t dataPin) {
    this->clockPin = clockPin;
    this->dataPin = dataPin;

    raw = 0;
    shift = 0;
    parity = 1;

    pinMode(clockPin, INPUT);
    digitalWrite(clockPin, HIGH);

    pinMode(dataPin, INPUT);
    digitalWrite(dataPin, HIGH);

    state = READ;
}

void PS2::test() {

    writeByte(0xFF);
    
    //Serial.println(queue.Size());

    uint8_t buf = 0xA6;

    readByte(buf);
    Serial.printf("read: %u\n", buf);

    readByte(buf);
    Serial.printf("read: %u\n", buf);

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
    state = READ;

    // clock and data should always be high and in input mode
    // at this point either after init or after write

    //digitalWrite(clockPin, HIGH);
    //digitalWrite(dataPin, HIGH);
    //pinMode(clockPin, INPUT_PULLUP);
    //pinMode(dataPin, INPUT_PULLUP);
    
    // interrupt
    
    while(queue.pull(data)) {
        // wait for read completion
        // mb add timeout?
    }
    
    return 0;
}

uint8_t PS2::writeByte(uint8_t data) {
    state = WRITE_START;

    raw = data;

    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    delayMicroseconds(100);

    state = WRITE;
    
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);

    pinMode(clockPin, INPUT_PULLUP);

    // interrupt

    while(state != READ) {
        // wait for write completion
    }

    // read ACK
    readByte(data);

    switch (data)
    {
        case 0xFA:
            return 0;
    
        default:
            // 0xFC: ERROR
            // 0xFE: NAC
            return 1;
    }
}

void PS2::int_on_clock() {

    switch(state) {
        case WRITE:
            int_write();
            break;

        case WRITE_START:
            // Self inflicted interrupt bypass
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
            if (parity != bit)
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
            digitalWrite(dataPin, HIGH);
            pinMode(dataPin, INPUT_PULLUP);
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