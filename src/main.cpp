//#include <Arduino.h>
#include "Core.h"

USB Usb;

dweedee::Core *core;

void setup() {
  core = new dweedee::Core(&Usb);
  Serial.println("setup() completed.");
}

void loop() {
  core->task();
//    Serial.println("loop() completed.");
}