#include <Arduino.h>

#include "trackpad.h"
TrackPad trackpad1(0);
TrackPad trackpad2(1);

//#include <Mouse.h>

void int_on_clock_1();
void int_on_clock_2();

void setup() {
    Serial.begin(256000);
    
    //Mouse.begin();

    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, LOW);

    delay(1500);
    Serial.printf("Clock %i Hz\n", F_CPU);

    Serial.println("setup");
    
    const uint32_t DATA_PIN_1  = PB9;
    const uint32_t CLOCK_PIN_1 = PB8;

    attachInterrupt(CLOCK_PIN_1, int_on_clock_1, FALLING);

    trackpad1.initialize(CLOCK_PIN_1, DATA_PIN_1);

    const uint32_t DATA_PIN_2  = PB7;
    const uint32_t CLOCK_PIN_2 = PB6;

    attachInterrupt(CLOCK_PIN_2, int_on_clock_2, FALLING);

    trackpad2.initialize(CLOCK_PIN_2, DATA_PIN_2);

    digitalWrite(PC13, HIGH);
    Serial.println("setup end");
}

void loop() {
    trackpad1.poll();
    trackpad2.poll();
}

void int_on_clock_1() {
    trackpad1.int_on_clock();
}

void int_on_clock_2() {
    trackpad2.int_on_clock();
}
