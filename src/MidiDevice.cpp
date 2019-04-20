#include "MidiDevice.h"


UsbMidiDevice::UsbMidiDevice(USBH_MIDI &usbMidi, UsbDeviceInfo &deviceInfo) {
    // TODO
//    this->usbMidi = &usbMidi;
//    this->deviceInfo = &deviceInfo;
}

void UsbMidiDevice::read() {
    //todo
}

void UsbMidiDevice::write() {
    //todo
}


SerialMidiDevice::SerialMidiDevice(HardwareSerial &serial) {
    this->serial = &serial;
}

void SerialMidiDevice::read() {
    //todo
}

void SerialMidiDevice::write() {
    //todo
}
