#ifndef DWEEDEE_UTILITY_H
#define DWEEDEE_UTILITY_H


namespace dweedee {

    // TODO: Migrate this into a global error handler class
    void halt();

    void haltBlinking(unsigned long interval);
    void haltBlinking(unsigned long interval, unsigned int ledPin);

    void haltBlinkingTimed(unsigned long timeOn, unsigned long timeOff);
    void haltBlinkingTimed(unsigned long timeOn, unsigned long timeOff, unsigned int ledPin);

}


#endif //DWEEDEE_UTILITY_H
