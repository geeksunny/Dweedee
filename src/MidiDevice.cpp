#include "MidiDevice.h"

namespace dweedee {

////////////////////////////////////////////////////////////////
// Class : UsbMidiDevice ///////////////////////////////////////
////////////////////////////////////////////////////////////////

UsbMidiDevice::UsbMidiDevice(USBH_MIDI *usbMidi, UsbDeviceInfo *deviceInfo)
    : usbMidi_(usbMidi), deviceInfo_(deviceInfo) {
  // TODO : Null checks?
}

void UsbMidiDevice::setDevice(USBH_MIDI *usbMidi) {
  // TODO : Null check?
  usbMidi_ = usbMidi;
}

MidiMessage *UsbMidiDevice::read(MidiReadHandler &handler) {
  // TODO: sysex handling
  uint8_t size;
  uint8_t buf[3];
  do {
    if ((size = usbMidi_->RecvData(buf)) > 0) {
      return new MidiMessage(buf, size);
    }
  } while (size > 0);
  return nullptr;
}

void UsbMidiDevice::write(MidiMessage *message) {
  usbMidi_->SendData(message->getBytes(), 0);
}

uint8_t UsbMidiDevice::getAddress() {
  return deviceInfo_->devAddress;
}

////////////////////////////////////////////////////////////////
// Class : SerialMidiDevice ////////////////////////////////////
////////////////////////////////////////////////////////////////

SerialMidiDevice::SerialMidiDevice(HardwareSerial &serial)
    : serial_(&serial), midi_(midi::MidiInterface<HardwareSerial>((HardwareSerial &) serial)) {
  // TODO : Null checks?
}

MidiMessage *SerialMidiDevice::read(MidiReadHandler &handler) {
  if (midi_.read()) {
    switch (midi_.getType()) {
      case midi::ActiveSensing:
      case midi::SystemExclusive:
        // TODO: Implement parsing of these types.
        break;
      default:return new MidiMessage(midi_.getType(), midi_.getChannel(), midi_.getData1(), midi_.getData2());
    }
  }
  return nullptr;
}

void SerialMidiDevice::write(MidiMessage *message) {
  serial_->write(message->getBytes(), message->getLength());
}

}
