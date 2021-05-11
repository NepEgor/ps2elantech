#include <Arduino.h>

#include "debug.h"

#include "trackpad.h"
const uint32_t DATA_PIN  = PB9;
const uint32_t CLOCK_PIN = PB8;
TrackPad trackpad;

void int_on_clock_1();

void setup() {
    Serial.begin(9600);
    
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);

    delay(1500);
    Serial.println("setup");

    attachInterrupt(CLOCK_PIN, int_on_clock_1, FALLING);

    trackpad.initialize(CLOCK_PIN, DATA_PIN);

    Serial.println("setup end");
}

void loop() {
    trackpad.poll();
}

void int_on_clock_1() {
    digitalWrite(PC13, LOW);

    trackpad.int_on_clock();

    digitalWrite(PC13, HIGH);
}