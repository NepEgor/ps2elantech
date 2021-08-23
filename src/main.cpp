#include <Arduino.h>

#include "debug.h"

//#include "trackpad.h"
const uint32_t DATA_PIN  = PB9;
const uint32_t CLOCK_PIN = PB8;
//TrackPad trackpad;

#include "PS2.h"
PS2 ps2;

void int_on_clock_1();

void setup() {
    Serial.begin(9600);
    
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);

    delay(1500);
    Serial.println("setup");

    attachInterrupt(CLOCK_PIN, int_on_clock_1, FALLING);

    //trackpad.initialize(CLOCK_PIN, DATA_PIN);

    ps2.initialize(CLOCK_PIN, DATA_PIN);

    Serial.println("test");

    ps2.test();

    Serial.println("setup end");
}

void loop() {
    //trackpad.poll();

    //delay(50);
}

volatile uint8_t blink = 1;

void int_on_clock_1() {
    digitalWrite(PC13, blink);
    blink = !blink;

    //trackpad.int_on_clock();

    ps2.int_on_clock();
}
