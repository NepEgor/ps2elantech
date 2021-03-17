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

void PS2::initialize(uint8 clockPin, uint8 dataPin) {
    this->clockPin = clockPin;
    this->dataPin = dataPin;

    raw = 0;
    shift = 0;
    parity = 1;

    start = 0;
    interval = 0;

    sliced_shift = 8;

    handleACK = false;

    low(clockPin);
    low(dataPin);

    setIdle();
}

State PS2::getState() {
    return state;
}

void PS2::setIdle() {
    low(clockPin);
    state = IDLE;
}

uint8 PS2::getIdle() {
    return state == IDLE;
}

uint8 PS2::sliced_command(uint16 command, bool wait) {
    do {
        if(sliced_shift < 0) {
            sliced_shift = 8;
            return 0;
        }
        else if(sliced_shift == 8) {
            if(!this->command(PS2_CMD_SETSCALE11, NULL)) {
                sliced_shift -= 2;
                command_part = (command >> sliced_shift) & 3;
            }
        }
        else { // 6 - 0
            if(!this->command(PS2_CMD_SETRES, &command_part)) {
                sliced_shift -= 2;
                command_part = (command >> sliced_shift) & 3;
            }
        }
    } while(wait);
    
    return 1;
}

uint8 PS2::command(uint16 command, uint8 *param, bool wait) {
    do {
        switch(state) {
            case IDLE:
                left_bytes = 0;
                send_bytes = PS2_SEND_BYTES(command);
                recv_bytes = PS2_RECV_BYTES(command);
            case WRITE_START:
                if(!left_bytes) {
                    writeByte(command & 0xFF);
                }
                else {
                    writeByte(param[left_bytes - 1]);
                }
                
                break;

            case READ_FINISH:
                if(!handleACK) {
                    if(!readByte(buf)) {
                        param[left_bytes] = buf;

                        ++left_bytes;

                        if(left_bytes < recv_bytes) {
                            state = IDLE;
                            readByte(buf);
                        }
                        else {
                            setIdle();
                            return 0;
                        }
                    }
                    break;
                }
                // else
            case WRITE_FINISH:
                if(!writeByte(0)) {
                    ++left_bytes;

                    if(left_bytes > send_bytes) {
                        if(!recv_bytes) {
                            setIdle();
                            return 0;
                        }
                        
                        left_bytes = 0;
                        state = IDLE;
                        readByte(buf);
                    }
                    else {
                        state = WRITE_START;
                    }
                }
                
                break;

            default:
                break;
        } // switch
    } while(wait);

    return 1;
}

uint8 PS2::readByte(uint8 &data) {
    switch(state) {
        case IDLE:
            state = READ;

            high(dataPin);
            high(clockPin);

            // interrupt
            break;

        case READ_FINISH:
            if(!queue.pull(data)){
                return 0;
            }

            break;

        default:
            break;
    }
    return 1;
}

uint8 PS2::writeByte(uint8 data) {
    switch(state) {
        case IDLE:
            state = WRITE_START;
            raw = data;
            handleACK = false;

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

        // handle ACK
        case WRITE_FINISH:
            handleACK = true;
            state = IDLE;
        case READ_FINISH:
            if(!readByte(buf)) {
                switch(buf){
                    case 0xFA: // TODO ACK
                    case 0xFC: // TODO ERROR
                    case 0xFE: // TODO NAC
                        break;
                    default:
                        break;
                }

                handleACK = false;
                return 0;
            }

            break;

        default:
            break;
    }

    return 1;
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
    uint8 bit = digitalRead(dataPin);

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
    uint8 bit = 0;

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