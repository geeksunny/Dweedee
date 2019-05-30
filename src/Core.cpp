#include "Core.h"
#include <PrintStream.h>    // swap with <ArduinoSTL.h>


namespace dweedee {

    Core::Core(USB *Usb) : usbMgr(new HotplugManager(Usb, this)) {
        //
    }

    void Core::task() {
        usbMgr->task();
    }

    void Core::onDevicesAdded(UsbDeviceInfo **added, short count) {
        for (short i = 0; i < count; ++i) {
            if (added[i] == nullptr) {
                continue;
            }
            USBH_MIDI *device = usbMgr->getUsbDevicePool()->getUsbMidi(added[i]->devAddress);
            if (device != nullptr) {
                midiDevices_.push_back(new UsbMidiDevice(*device, *added[i]));
            }
        }
    }

    void Core::onDevicesRemoved(UsbDeviceInfo **removed, short count) {
        // TODO: Remove / disable UsbMidiDevices matching contents of removed[]
        Serial << "!! onDevicesRemoved :: Count: " << count << endl << "!!!!!!!!!!!!!" << endl << hex;
        for (short i = 0; i < count; i++) {
            Serial << "DISCONNECT EVENT" << endl
                   << "Device:        "  << removed[i]->productName << " | PID: " << removed[i]->pid << endl
                   << "Manufacturer:  "  << removed[i]->vendorName  << " | VID: " << removed[i]->vid << endl
                   << "Removing `UsbDeviceInfo` from index." << endl << endl;
        }
        Serial << dec;
    }

}
