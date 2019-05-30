#include "MidiDevice.h"


namespace dweedee {

    UsbMidiDevice::UsbMidiDevice(USBH_MIDI &usbMidi, UsbDeviceInfo &deviceInfo) : usbMidi_(&usbMidi), deviceInfo_(&deviceInfo) {
        // TODO
    }

    void UsbMidiDevice::read() {
        //todo
    }

    void UsbMidiDevice::write() {
        //todo
    }

    uint8_t UsbMidiDevice::getAddress() {
        return deviceInfo_->devAddress;
    }


    SerialMidiDevice::SerialMidiDevice(HardwareSerial &serial) : serial_(&serial) {
        //
    }

    void SerialMidiDevice::read() {
        //todo
    }

    void SerialMidiDevice::write() {
        //todo
    }

}
