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

void printParam(uint8_t *param, uint8_t len);

void PS2::test() {

    uint8_t param[3];

    Serial.printf("PS2_CMD_RESET_BAT %u\n", command(PS2_CMD_RESET_BAT, param));
    printParam(param, PS2_RECV_BYTES(PS2_CMD_RESET_BAT));

    Serial.printf("PS2_CMD_GETID %u\n", command(PS2_CMD_GETID, param));
    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETID));

    Serial.printf("PS2_CMD_RESET_DIS %u\n", command(PS2_CMD_RESET_DIS));

    Serial.println("Elantech magic knock");

    command(PS2_CMD_RESET_DIS);

    command(PS2_CMD_DISABLE);
    command(PS2_CMD_SETSCALE11);
    command(PS2_CMD_SETSCALE11);
    command(PS2_CMD_SETSCALE11);
    command(PS2_CMD_GETINFO, param);

    printParam(param, PS2_RECV_BYTES(PS2_CMD_GETINFO));
}

uint8_t PS2::sliced_command(uint8_t command) {
    
    writeByte(PS2_CMD_SETSCALE11 & 0xFF);
    
    for (int8_t shift = 6; shift >= 0; shift -= 2) {
        writeByte(PS2_CMD_SETRES & 0xFF);
        writeByte((command >> shift) & 0b11);
    }
    
    return 0;
}

uint8_t PS2::readPacket(uint8_t *packet, uint8_t size) {
    
    for (uint8_t i = 0; i < size; ++i) {
        if (readByte(packet[i]))
            return i + 1;
    }

    return 0;
}

uint8_t PS2::command(uint16_t command, uint8_t *param) {
    
    if (writeByte(command & 0xFF))
        return 1;
    
    uint8_t N = PS2_SEND_BYTES(command);
    for (uint8_t i = 0; i < N; ++i) {
        if (writeByte(param[i]))
            return 2;
    }
    
    N = PS2_RECV_BYTES(command);
    for (uint8_t i = 0; i < N; ++i) {
        if (readByte(param[i]))
            return 3;
    }

    return 0;
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
        // mb add timeout?
    }

    // read ACK
    readByte(data);

    //Serial.printf("ACK: %X\n", data);

    switch (data)
    {
        case 0xFA: // ACK
            return 0;
    
        case 0xFC: // ERROR
            return 1;

        case 0xFE: // NAC
            return 2;

        default:
            return -1;
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