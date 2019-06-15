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
        refreshLength();
    }

    void MidiMessage::refreshLength() {
        if (data_.basicType < 0xF) {
            // Basic
            switch (data_.basicType) {
                case dweedee::MidiType::NoteOff:
                case dweedee::MidiType::NoteOn:
                case dweedee::MidiType::PolyAfterTouch:
                case dweedee::MidiType::ControlChange:
                case dweedee::MidiType::PitchBend:
                    length_ = 3;
                    break;
                case dweedee::MidiType::ChannelAfterTouch:
                case dweedee::MidiType::ProgramChange:
                    length_ = 2;
                    break;
                case dweedee::MidiType::Invalid:
                default:
                    length_ = 0;
            }
        } else {
            // Extended
            switch (data_.extendedType) {
                case dweedee::MidiType::SongPosition:
                    length_ = 3;
                    break;
                case dweedee::MidiType::MTC:
                case dweedee::MidiType::SongSelect:
                    length_ = 2;
                    break;
                case dweedee::MidiType::Clock:
                case dweedee::MidiType::Start:
                case dweedee::MidiType::Continue:
                case dweedee::MidiType::Stop:
                case dweedee::MidiType::ActiveSensing:
                case dweedee::MidiType::Reset:
                    length_ = 1;
                    break;
                case dweedee::MidiType::Tune:
                case dweedee::MidiType::SysExEnd:
                case dweedee::MidiType::SysEx:
                    break;
                default:
                    length_ = 0;
            }
        }
    }

    void MidiMessage::setChannel(uint8_t channel) {
        // TODO: Figure out rules for channel, should value passed in be 0-based or 1-based?
        data_.channel = channel;
    }

    void MidiMessage::setType(dweedee::MidiType type) {
        if (data_.basicType < 0xF) {
            // Basic
            switch (type) {
                case dweedee::NoteOff:
                case dweedee::NoteOn:
                case dweedee::PolyAfterTouch:
                case dweedee::ControlChange:
                case dweedee::PitchBend:
                case dweedee::ChannelAfterTouch:
                case dweedee::ProgramChange:
                case dweedee::Invalid:
                    data_.basicType = type;
                    break;
                default:
                    data_.basicType = Invalid;
            }
        } else {
            // Extended
            switch (type) {
                case dweedee::SysEx:
                case dweedee::MTC:
                case dweedee::SongPosition:
                case dweedee::SongSelect:
                case dweedee::Tune:
                case dweedee::SysExEnd:
                case dweedee::Clock:
                case dweedee::Start:
                case dweedee::Continue:
                case dweedee::Stop:
                case dweedee::ActiveSensing:
                case dweedee::Reset:
                    data_.extendedType = type;
                    break;
                default:
                    data_.extendedType = Invalid;
            }
        }
        refreshLength();
    }

    void MidiMessage::setType(midi::MidiType type) {
        switch (type) {
            // Basic
            case midi::InvalidType:
            case midi::NoteOff:
            case midi::NoteOn:
            case midi::AfterTouchPoly:
            case midi::ControlChange:
            case midi::ProgramChange:
            case midi::AfterTouchChannel:
            case midi::PitchBend:
                data_.extendedType = type + data_.channel;
                break;
            // Extended
            case midi::SystemExclusive:
            case midi::TimeCodeQuarterFrame:
            case midi::SongPosition:
            case midi::SongSelect:
            case midi::TuneRequest:
            case midi::Clock:
            case midi::Start:
            case midi::Continue:
            case midi::Stop:
            case midi::ActiveSensing:
            case midi::SystemReset:
                data_.extendedType = type;
                break;
            default:
                data_.basicType = Invalid;
        }
        refreshLength();
    }

    void MidiMessage::setData(uint8_t data1) {
        data_.data1 = data1;
    }

    void MidiMessage::setData(uint8_t data1, uint8_t data2) {
        data_.data1 = data1;
        data_.data2 = data2;
    }

    uint8_t *MidiMessage::getBytes() {
        return data_.bytes;
    }

    uint8_t MidiMessage::getLength() {
        return length_;
    }

}
