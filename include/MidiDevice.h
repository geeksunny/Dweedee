#ifndef DWEEDEE_MIDIDEVICE_H
#define DWEEDEE_MIDIDEVICE_H

#include <SPI.h>
#include <Usb.h>
#include <usbh_midi.h>

#include "UsbDeviceInfo.h"


namespace dweedee {

    class MidiDevice {

    public:
        virtual void read() = 0;
        virtual void write() = 0;

    };


    class UsbMidiDevice : public MidiDevice {

        USBH_MIDI *usbMidi_;
        UsbDeviceInfo *deviceInfo_;

    public:

        UsbMidiDevice(USBH_MIDI &usbMidi, UsbDeviceInfo &deviceInfo);
        void read() override;
        void write() override;

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
