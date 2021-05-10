#include <Arduino.h>

#include "debug.h"

//#include <USBComposite.h>
//USBCompositeSerial CSerial;

/*
const uint8 reportDescription[] = {
   HID_MOUSE_REPORT_DESCRIPTOR(),
};

USBHID HID;
HIDMouse hid_mouse(HID);
*/

#include "trackpad.h"
#define DATA_PIN PB9
#define CLOCK_PIN PB8
TrackPad trackpad;

void int_on_clock_1();

void setup() {
    CSerial.begin(9600);
    //HID.begin(CSerial, reportDescription, sizeof(reportDescription));
    //while(!USBComposite);

    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);

    delay(1500);
    CSerial.println("setup");

    attachInterrupt(CLOCK_PIN, int_on_clock_1, FALLING);

    trackpad.initialize(CLOCK_PIN, DATA_PIN);

    CSerial.println("setup end");
}

void loop() {
/*    uint8 param[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    if(!ps2.command(0x03EB, param)){
        for(uint8 i = 0; i < 3; ++i) {
            CSerial.println(param[i], BIN);
        }
    }

    delay(5);*/
}

void int_on_clock_1() {
    digitalWrite(PC13, LOW);

    trackpad.int_on_clock();

    digitalWrite(PC13, HIGH);
}