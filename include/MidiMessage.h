#ifndef DWEEDEE_MIDIMESSAGE_H
#define DWEEDEE_MIDIMESSAGE_H


#include <MIDI.h>


#define MIDI_BYTE_MAX_LENGTH 3


namespace dweedee {

    struct MidiType {
        enum Basic {
            Invalid             = 0x0,
            NoteOff             = 0x8,
            NoteOn              = 0x9,
            PolyAfterTouch      = 0xA,
            ControlChange       = 0xB,
            ProgramChange       = 0xC,
            ChannelAfterTouch   = 0xD,
            PitchBend           = 0xE,
        };
        enum Extended {
            SysEx               = 0xF0,
            MTC                 = 0xF1,
            SongPosition        = 0xF2,
            SongSelect          = 0xF3,
            Tune                = 0xF6,
            SysExEnd            = 0xF7,
            Clock               = 0xF8,
            Start               = 0xFA,
            Continue            = 0xFB,
            Stop                = 0xFC,
            ActiveSensing       = 0xFE,
            Reset               = 0xFF
        };
    };


    struct MidiPacket {
        union {
            struct {
                uint8_t channel : 4;
                uint8_t basicType : 4;
                uint8_t data1 : 7;
                uint8_t data1reserved : 1;  // must be zero
                uint8_t data2 : 7;
                uint8_t data2reserved : 1;  // must be zero
            } __attribute__((packed));
            struct {
                uint8_t extendedType : 8;
            } __attribute__((packed));
            uint8_t bytes[3] = {0,0,0};
        };
        MidiPacket() = default;
    } __attribute__((packed));


    class MidiMessage {

        MidiPacket data_;
        uint8_t length_;

    public:
        MidiMessage(uint8_t bytes[], uint8_t length);
        MidiMessage(midi::MidiType midiType, uint8_t midiChannel, uint8_t data1, uint8_t data2);
        void setChannel(uint8_t channel);
        void setType(MidiType::Basic type);
        void setType(MidiType::Extended type);
        void setData(uint8_t data1);
        void setData(uint8_t data1, uint8_t data2);
        uint8_t *getBytes();
        uint8_t getLength();

    };

}

#endif //DWEEDEE_MIDIMESSAGE_H
