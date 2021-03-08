#include "PS2.h"
#include "debug.h"
#define DATA_PIN PB9
#define CLOCK_PIN PB8

#ifdef DEBUG
#include <USBComposite.h>
USBCompositeSerial CSerial;
#endif

const uint8_t reportDescription[] = {
   HID_MOUSE_REPORT_DESCRIPTOR(),
};

//USBHID HID;
//HIDMouse hid_mouse(HID);

PS2 ps2(CLOCK_PIN, DATA_PIN);

void mouse_init() {
    uint8_t param[3] = {0xFF, 0xFF, 0xFF};

    ps2.commandWait(0x02FF, param);

    for(uint8_t i = 0; i < 2; ++i) {
        CSerial.println(param[i], BIN);
        param[i] = 0xFF;
    }

    ps2.commandWait(0x01F2, param);

    for(uint8_t i = 0; i < 1; ++i) {
        CSerial.println(param[i], BIN);
    }

}

void setup() {
    CSerial.begin(9600);
    //HID.begin(CSerial, reportDescription, sizeof(reportDescription));
    while(!USBComposite);

    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);

    delay(5000);
    CSerial.println("setup");

    ps2.initialize();

    attachInterrupt(CLOCK_PIN, int_on_clock_1, FALLING);

    mouse_init();
    
    CSerial.println("setup end");
}

void loop() {
/*    uint8_t param[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    if(!ps2.command(0x03EB, param)){
        for(uint8_t i = 0; i < 3; ++i) {
            CSerial.println(param[i], BIN);
        }
    }

    delay(5);*/
}

void int_on_clock_1() {
    digitalWrite(PC13, LOW);

    ps2.int_on_clock();

    digitalWrite(PC13, HIGH);
}