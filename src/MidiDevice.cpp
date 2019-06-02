#include "MidiDevice.h"


namespace dweedee {


    UsbMidiDevice::UsbMidiDevice(USBH_MIDI *usbMidi, UsbDeviceInfo *deviceInfo) : usbMidi_(usbMidi), deviceInfo_(deviceInfo) {
        // TODO : Null checks?
    }

    void UsbMidiDevice::setDevice(USBH_MIDI *usbMidi) {
        // TODO : Null check?
        usbMidi_ = usbMidi;
    }

    MidiMessage *UsbMidiDevice::read() {
        //todo
    }

    void UsbMidiDevice::write(MidiMessage *message) {
        //todo
    }

    uint8_t UsbMidiDevice::getAddress() {
        return deviceInfo_->devAddress;
    }


    SerialMidiDevice::SerialMidiDevice(HardwareSerial &serial) : serial_(&serial) {
        // TODO : Null checks?
    }

    MidiMessage *SerialMidiDevice::read() {
        //todo
    }

    void SerialMidiDevice::write(MidiMessage *message) {
        //todo
    }


}
