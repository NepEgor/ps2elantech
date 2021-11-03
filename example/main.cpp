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
    
    const uint8_t DATA_PIN_1  = PB9;
    const uint8_t CLOCK_PIN_1 = PB8;

    attachInterrupt(CLOCK_PIN_1, int_on_clock_1, FALLING);

    trackpad1.initialize(CLOCK_PIN_1, DATA_PIN_1);

    const uint8_t DATA_PIN_2  = PB7;
    const uint8_t CLOCK_PIN_2 = PB6;

    attachInterrupt(CLOCK_PIN_2, int_on_clock_2, FALLING);

    trackpad2.initialize(CLOCK_PIN_2, DATA_PIN_2);

    digitalWrite(PC13, HIGH);
    Serial.println("setup end");
}

uint8_t tevent_size;
TouchEvent tevent[5];

void loop() {
    int8_t ret = trackpad1.poll(tevent, tevent_size);
    
    if (ret > 0)
    {
        for (uint8_t i = 0; i < tevent_size; ++i)
        {
            switch (tevent[i].type)
            {
                case TET_DOWN:

                    break;

                case TET_MOVE:

                    break;
                
                case TET_UP:

                    break;

                default:
                    break;
            }
        }
    }
    
    trackpad2.poll(tevent, tevent_size);
}

void int_on_clock_1() {
    trackpad1.int_on_clock();
}

void int_on_clock_2() {
    trackpad2.int_on_clock();
}
