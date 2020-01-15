#ifndef DWEEDEE_MIDIDEVICE_H
#define DWEEDEE_MIDIDEVICE_H

#include <SPI.h>
#include <Usb.h>
#include <usbh_midi.h>
#include <MIDI.h>

#include "UsbDeviceInfo.h"
#include "MidiMessage.h"

namespace dweedee {

class MidiDevice;   // Predeclared for use in MidiReadHandler

class MidiReadHandler {

 public:
  virtual void onMidiData(MidiDevice *device, MidiMessage *message) = 0;

};

class MidiDevice {

  bool enabled_ = true;

 public:
  virtual MidiMessage *read(MidiReadHandler &handler) = 0;
  virtual void write(MidiMessage *message) = 0;

  virtual uint8_t getAddress() { return 0; }

  bool isEnabled() { return enabled_; }
  void setEnabled(bool enabled) { enabled_ = enabled; }

};

class UsbMidiDevice : public MidiDevice {

  USBH_MIDI *usbMidi_;
  UsbDeviceInfo *deviceInfo_;

 public:

  UsbMidiDevice(USBH_MIDI *usbMidi, UsbDeviceInfo *deviceInfo);
  void setDevice(USBH_MIDI *usbMidi);
  MidiMessage *read(MidiReadHandler &handler) override;
  void write(MidiMessage *message) override;
  uint8_t getAddress() override;

};

class SerialMidiDevice : public MidiDevice {

  HardwareSerial *serial_;
  midi::MidiInterface<HardwareSerial> midi_;

 public:
  explicit SerialMidiDevice(HardwareSerial &serial);
  MidiMessage *read(MidiReadHandler &handler) override;
  void write(MidiMessage *message) override;

};

}

#endif //DWEEDEE_MIDIDEVICE_H
