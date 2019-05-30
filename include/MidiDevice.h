#ifndef DWEEDEE_MIDIDEVICE_H
#define DWEEDEE_MIDIDEVICE_H

#include <SPI.h>
#include <Usb.h>
#include <usbh_midi.h>

#include "UsbDeviceInfo.h"


namespace dweedee {

    class MidiDevice {

        bool enabled_ = true;

    public:
        virtual void read() = 0;
        virtual void write() = 0;

        virtual uint8_t getAddress() { return 0; }

        bool isEnabled() { return enabled_; }
        void setEnabled(bool enabled) { enabled_ = enabled;}

    };


    class UsbMidiDevice : public MidiDevice {

        USBH_MIDI *usbMidi_;
        UsbDeviceInfo *deviceInfo_;

    public:

        UsbMidiDevice(USBH_MIDI &usbMidi, UsbDeviceInfo &deviceInfo);
        void read() override;
        void write() override;
        uint8_t getAddress() override;

    };


    class SerialMidiDevice : public MidiDevice {

        HardwareSerial *serial_;

    public:
        SerialMidiDevice(HardwareSerial &serial);
        void read() override;
        void write() override;

    };

}


#endif //DWEEDEE_MIDIDEVICE_H
