 #include "debug.h"

 #include <Arduino.h>
 
 void blink() {
    static uint8_t state = 0;
    digitalWrite(PC13, state);
    state = !state;
}