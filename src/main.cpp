//#include <Arduino.h>
#include "Core.h"


Core core;


void setup() {
    core = Core();
    Serial.println("setup");
}

void loop() {
    core.task();
    Serial.println("loop");
}