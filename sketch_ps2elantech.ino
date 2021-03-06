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

//#include "queue.h"
//Queue queue;

//USBHID HID;
//HIDMouse hid_mouse(HID);

PS2 ps2(CLOCK_PIN, DATA_PIN);

void mouse_init() {
    /*State ps2state;
    while(ps2state != WRITE_FINISH) {
        ps2.writeByte(0xFF);
        ps2state = ps2.getState();
    }*/

    uint8_t param[3] = {0xFF, 0xFF, 0xFF};

    do {
        ps2.command(0x02FF, param);
        CSerial.println(ps2.getState());
    } while(!ps2.getIdle());

    for(uint8_t i = 0; i < 3; ++i) {
        CSerial.println(param[i], BIN);
        param[i] = 0xFF;
    }

    delay(2000);

    do {
        ps2.command(0x01F2, param);
        CSerial.println(ps2.getState());
    } while(!ps2.getIdle());

    for(uint8_t i = 0; i < 3; ++i) {
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

    delay(100);

    mouse_init();
    
    CSerial.println("setup end");
}

void loop() {
    queue_elem_t buf;
    /*
    if(!ps2.queue.pull(buf)) {
        CSerial.println(buf, BIN);
    }
    */
    /*if(ps2.Idle()) {
        ps2.writeByte(0xEB);
    }*/
    /*
    if(!ps2.readByte(&buf)) {
        CSerial.println(buf, BIN);
        ps2.setIdle();
    }*/
}

void int_on_clock_1() {
    digitalWrite(PC13, LOW);

    ps2.int_on_clock();

    digitalWrite(PC13, HIGH);
}