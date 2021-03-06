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

    //CSerial.println(ps2.getState());

    delay(100);

    State ps2state;
    do{
        ps2.writeByte(0xFF);
        ps2state = ps2.getState();
        //CSerial.println(millis());
    } while(ps2state != WRITE && ps2state != IDLE);

    //while(ps2.getState() != IDLE);
    ps2.readByte();
    //ps2.readByte();
    /*while(ps2.getState() != IDLE);
    CSerial.println(ps2.getState());
    
    CSerial.println(ps2.readByte());
    CSerial.println(ps2.getState());

    CSerial.println(ps2.readByte());
    CSerial.println(ps2.getState());

    CSerial.println(ps2.readByte());
    CSerial.println(ps2.getState());
    */
    CSerial.println("setup end");

    //while(ps2.getState() != IDLE);

}

void loop() {
    //CSerial.println(ps2.shift);
    //if(ps2.write_shift) CSerial.println(ps2.write_shift);
    //CSerial.println(ps2.getState());
    queue_elem_t buf;
    if(!ps2.queue.pull(buf)){
        CSerial.println(buf, BIN);
    }
    
}

void int_on_clock_1(){
    //digitalWrite(PC13, LOW);
    ps2.int_on_clock();
}