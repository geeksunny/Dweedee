#ifndef DWEEDEE_MIDIMESSAGE_H
#define DWEEDEE_MIDIMESSAGE_H


#include <stdint-gcc.h>


#define MIDI_BYTE_MAX_LENGTH 3


typedef uint8_t DataByte;


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


    struct StatusByte {
        StatusByte() : value(0) {};

        union {
            struct {
                uint8_t channel : 4;
                uint8_t basicType : 4;
            } __attribute__((packed));
            struct {
                uint8_t extendedType : 8;
            } __attribute__((packed));
            uint8_t value;
        };
    } __attribute__((packed));


    class MidiMessage {

        StatusByte statusByte_;
        DataByte byte1_;
        DataByte byte2_;

    public:
        MidiMessage(uint8_t bytes[], uint8_t length);
        void setChannel(uint8_t channel);
        void setType(MidiType::Basic type);
        void setType(MidiType::Extended type);

    };

}

#endif //DWEEDEE_MIDIMESSAGE_H
