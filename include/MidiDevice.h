//#include <Arduino.h>
#include <SPI.h>
#include <Usb.h>
#include <usbh_midi.h>

#ifndef DWEEDEE_MIDIDEVICE_H
#define DWEEDEE_MIDIDEVICE_H


struct MidiDeviceInfo {
    uint8_t devAddress;
    uint16_t vid;
    uint16_t pid;
    char* vendorName;
    char* productName;
};


class MidiDevice {

public:
    virtual void read() = 0;
    virtual void write() = 0;

};


class UsbMidiDevice : MidiDevice {

    USBH_MIDI *usbMidi;
    MidiDeviceInfo *deviceInfo;

public:

    UsbMidiDevice(USBH_MIDI &usbMidi, MidiDeviceInfo &deviceInfo);
    void read() override;
    void write() override;

};


class SerialMidiDevice : MidiDevice {

    HardwareSerial *serial;

public:
    SerialMidiDevice(HardwareSerial &serial);
    void read() override;
    void write() override;

};


#endif //DWEEDEE_MIDIDEVICE_H
