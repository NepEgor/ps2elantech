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

State PS2::getState(){
    return state;
}

void PS2::setIdle(){
    //low(clockPin);
    //high(clockPin);
    state = IDLE;
    //digitalWrite(PC13, HIGH);
}

void PS2::command(uint16_t command, uint8_t *param){

}

uint8_t PS2::readByte() {
    uint16_t buf;
    /*if(!read_queue.pull(buf))
        return buf & 0xFF;
*/
    //while(state != IDLE);

    delay(10);

    while(state != IDLE);

    state = READ;

    high(dataPin);
    high(clockPin);

    // interrupt
/*    
    while(state != IDLE);

    if(!read_queue.pull(buf))
        return buf & 0xFF;

    else*/ return 0xFF; // error

}

/*
void PS2::writeByte(uint8_t data) {
    while(state != IDLE);

    state = WRITE;
    raw = data;
    //high(clockPin);
    //delayMicroseconds(100);
    //delay(100);

    low(clockPin);
    high(dataPin);

    delayMicroseconds(100);
    //delay(100);

    low(dataPin);

    //delay(10);

    high(clockPin);

}
*/

void PS2::writeByte(uint8_t data) {
    switch(state){
        case IDLE:
            state = WRITE_START;
            raw = data;

            low(clockPin);
            high(dataPin);

            start = micros();
            interval = 100;

            break;

        case WRITE_START:
            if(micros() - start >= interval){
                interval = 0;
                state = WRITE;

                low(dataPin);
                high(clockPin);
            }

            break;
    }
    
    // interrupt

}

void PS2::int_on_clock(){

    digitalWrite(PC13, LOW);

    switch(state){
        case READ:
            //digitalWrite(PC13, LOW);
            int_read();
            break;

        case WRITE:
            //digitalWrite(PC13, LOW);
            int_write();
            break;

        default:
            break;
    }

    digitalWrite(PC13, HIGH);    
}

void PS2::int_read(){
    uint8_t bit = digitalRead(dataPin);

    /*
        0 - start bit
        1-8 - data
        9 - parity
        10 - stop
    */
    switch(shift){
        
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
            //queue.push((raw >> 1) & 0xFF);
            queue.push(raw);
            setIdle();
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

void PS2::int_write(){
    uint8_t bit = 0;
    //digitalWrite(PC13, LOW);

    /*
        start bit sent already
        0-7 - data
        8 - parity
        9 - stop
        10 - read ack
    */
    if(shift >= 0 && shift <= 7){
        bit = (raw >> shift) & 1;
        parity ^= bit;
        digitalWrite(dataPin, bit);
        ++shift;
    }
    else if(shift == 8){ // parity
        digitalWrite(dataPin, parity);
        //digitalWrite(dataPin, (~__builtin_parity(raw)) & 1);
        ++shift;
    }
    else if(shift == 9){
        high(dataPin);
        ++shift;
    }
    else if(shift == 10){
        bit = digitalRead(dataPin);
        setIdle();
        // reset
        raw = 0;
        shift = 0;
        parity = 1;
    }
}