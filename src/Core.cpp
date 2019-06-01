#include "Core.h"
#include <PrintStream.h>    // swap with <ArduinoSTL.h>


namespace dweedee {

    Core::Core(USB *Usb) : usbMgr(new HotplugManager(Usb, this)) {
        //
    }

    void Core::task() {
        usbMgr->task();
    }

    MidiDevice *Core::findMidiDevice(uint8_t devAddr) {
        for (auto it = midiDevices_.begin(); it < midiDevices_.end(); ++it) {
            if ((*it)->getAddress() == devAddr) {
                return *it;
            }
        }
        return nullptr;
    }

    void Core::onDevicesAdded(UsbDeviceInfo **added, short count) {
        for (short i = 0; i < count; ++i) {
            if (added[i] == nullptr) {
                continue;
            }
            USBH_MIDI *midi = usbMgr->getUsbDevicePool()->getUsbMidi(added[i]->devAddress);
            UsbMidiDevice *device = reinterpret_cast<UsbMidiDevice*>(findMidiDevice(added[i]->devAddress));
            if (device != nullptr) {
                device->setDevice(midi);
                device->setEnabled(true);
            } else if (midi != nullptr && usbMgr->isUsbMidi(added[i]->devAddress)) {
                midiDevices_.push_back(new UsbMidiDevice(*midi, *added[i]));
            }
        }
    }

    void Core::onDevicesRemoved(UsbDeviceInfo **removed, short count) {
        for (short i = 0; i < count; i++) {
            MidiDevice *device = findMidiDevice(removed[i]->devAddress);
            if (device != nullptr) {
                device->setEnabled(false);
            }
        }
    }

}
