#include "Utility.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <HID.h>


#define DEFAULT_LED_PIN 13


namespace dweedee {

    void halt() {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        cli();
        sleep_mode();
    }

    void haltBlinking(unsigned long interval) {
        haltBlinkingTimed(interval, interval, DEFAULT_LED_PIN);
    }

    void haltBlinking(unsigned long interval, unsigned int ledPin) {
        haltBlinkingTimed(interval, interval, ledPin);
    }

    void haltBlinkingTimed(unsigned long timeOn, unsigned long timeOff) {
        haltBlinkingTimed(timeOn, timeOff, DEFAULT_LED_PIN);
    }

    void haltBlinkingTimed(unsigned long timeOn, unsigned long timeOff, unsigned int ledPin) {
        bool ledOn = digitalRead(ledPin == HIGH);
        while (true) {
            ledOn = !ledOn;
            digitalWrite(ledPin, (ledOn) ? HIGH : LOW);
            delay(ledOn ? timeOn : timeOff);
        }
    }

}
