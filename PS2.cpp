#include "PS2.h"

void high(int pin) {
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

void low(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

PS2::PS2(int clockPin, int dataPin) {
    this->clockPin = clockPin;
    this->dataPin = dataPin;

    raw = 0;
    shift = 0;
    parity = 1;

    start = 0;
    interval = 0;

    state = IDLE;
}

void PS2::initialize() {
    low(clockPin);
    low(dataPin);

    setIdle();
}

State PS2::getState() {
    return state;
}

void PS2::setIdle() {
    //low(clockPin);
    state = IDLE;
}

uint8_t PS2::getIdle() {
    return state == IDLE;
}

void PS2::commandWait(uint16_t command, uint8_t *param){
    while(this->command(command, param)) { }
}

// write command byte
// command format 0xARCC
// A - number of args
// R - number of returns
// CC - command
// command args and returns in params array
uint8_t PS2::command(uint16_t command, uint8_t *param) {
    uint8_t buf;
    switch(state) {
        case IDLE:
            left_bytes = 0;
            send_bytes = (command >> 12) & 0xf;
            recv_bytes = (command >> 8) & 0xf;
        case WRITE_START:
        case WRITE:
            if(!left_bytes) {
                writeByte(command & 0xFF);
            }
            else {
                writeByte(param[left_bytes - 1]);
            }
            
            break;

        case WRITE_FINISH:
            ++left_bytes;
            if(left_bytes > send_bytes) {
                left_bytes = 0;
                startReading();
            }

            break;

        case READ_FINISH:
            if(!readByte(buf)) {
                if(!left_bytes) {
                    switch(buf){
                        case 0xFA: // TODO ACK
                        case 0xFC: // TODO ERROR
                        case 0xFE: // TODO NAC
                            break;
                        default:
                            break;
                    }
                }
                else{
                    param[left_bytes - 1] = buf;
                }

                ++left_bytes;

                if(left_bytes <= recv_bytes) {
                    startReading();
                }
                else {
                    setIdle();
                    return 0;
                }
            }

            break;
    }

    return 1;
}

void PS2::startReading() {
    switch(state) {
        case READ_FINISH:
        case WRITE_FINISH:
        case IDLE:
            state = READ;

            high(dataPin);
            high(clockPin);

            // interrupt
            break;
    }
}

uint8_t PS2::readByte(uint8_t &data) {
    if(!queue.pull(data)){
        return 0;
    }
    
    return 1;
}

void PS2::writeByte(uint8_t data) {
    switch(state) {
        case WRITE_FINISH:
        case READ_FINISH:
        case IDLE:
            state = WRITE_START;
            raw = data;

            low(clockPin);
            high(dataPin);

            start = micros();
            interval = 100;

            break;

        case WRITE_START:
            if(micros() - start >= interval) {
                interval = 0;
                state = WRITE;

                low(dataPin);
                high(clockPin);
            }

            // interrupt

            break;
    }

}

void PS2::int_on_clock() {

    switch(state) {
        case READ:
            int_read();
            break;

        case WRITE:
            int_write();
            break;

        default:
            break;
    }

}

void PS2::int_read() {
    uint8_t bit = digitalRead(dataPin);

    /*
        0 - start bit
        1-8 - data
        9 - parity
        10 - stop
    */
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
            state = READ_FINISH;
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

    /*
        start bit sent already
        0-7 - data
        8 - parity
        9 - stop
        10 - read ack
    */
    if(shift >= 0 && shift <= 7) {
        bit = (raw >> shift) & 1;
        parity ^= bit;
        digitalWrite(dataPin, bit);
        ++shift;
    }
    else if(shift == 8) { // parity
        digitalWrite(dataPin, parity);
        ++shift;
    }
    else if(shift == 9) {
        high(dataPin);
        ++shift;
    }
    else if(shift == 10) {
        bit = digitalRead(dataPin);
        state = WRITE_FINISH;
        // reset
        raw = 0;
        shift = 0;
        parity = 1;
    }
}