#include <cstring>
#include "MidiMessage.h"


namespace dweedee {

    MidiMessage::MidiMessage(uint8_t bytes[], uint8_t length) {
        switch (length) {
            case 1:
                statusByte_.value = bytes[0];
            case 2:
                byte1_ = bytes[1];
            case 3:
            default:
                byte2_ = bytes[2];
                break;
            case 0:
                break;
        }
    }

    void MidiMessage::setChannel(uint8_t channel) {
        statusByte_.channel = channel;
    }

    void MidiMessage::setType(dweedee::MidiType::Basic type) {
        statusByte_.basicType = type;
    }

    void MidiMessage::setType(dweedee::MidiType::Extended type) {
        statusByte_.extendedType = type;
    }

}
