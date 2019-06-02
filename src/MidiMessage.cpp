#include "MidiMessage.h"


namespace dweedee {

    MidiMessage::MidiMessage(uint8_t bytes[], uint8_t length) {
        switch (length) {
            default:
                length = 3;
            case 3:
                data_.bytes[2] = bytes[2];
            case 2:
                data_.bytes[1] = bytes[1];
            case 1:
                data_.bytes[0] = bytes[0];
                break;
            case 0:
                break;
        }
        length_ = length;
    }

    MidiMessage::MidiMessage(midi::MidiType midiType, uint8_t midiChannel, uint8_t data1, uint8_t data2) {
        data_.bytes[0] = (midiType | ((midiChannel - 1) & 0x0f));
        data_.bytes[1] = data1;
        data_.bytes[2] = data2;
        length_ = (data2 > 0) ? 3 : (data1 > 0) ? 2 : 1;
    }

    void MidiMessage::setChannel(uint8_t channel) {
        data_.channel = channel;
        if (length_ == 0) {
            length_ = 1;
        }
    }

    void MidiMessage::setType(dweedee::MidiType::Basic type) {
        data_.basicType = type;
        if (length_ == 0) {
            length_ = 1;
        }
    }

    void MidiMessage::setType(dweedee::MidiType::Extended type) {
        data_.extendedType = type;
        if (length_ == 0) {
            length_ = 1;
        }
    }

    void MidiMessage::setData(uint8_t data1) {
        data_.data1 = data1;
        length_ = 2;
    }

    void MidiMessage::setData(uint8_t data1, uint8_t data2) {
        data_.data1 = data1;
        data_.data2 = data2;
        length_ = 3;
    }

    uint8_t *MidiMessage::getBytes() {
        return data_.bytes;
    }

    uint8_t MidiMessage::getLength() {
        return length_;
    }

}
