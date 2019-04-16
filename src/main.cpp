//#include <Arduino.h>
#include "Core.h"

USB Usb;

Core *core;


void setup() {
    core = new Core(&Usb);
    Serial.println("setup() completed.");
}

void loop() {
    core->task();
//    Serial.println("loop() completed.");
}