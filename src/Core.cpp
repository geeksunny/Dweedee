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
        // TODO: Create / enable UsbMidiDevices with contents of added[]
        Serial << "!! onDevicesAdded :: Count: " << count << endl << "!!!!!!!!!!!!!" << endl << hex;
        for (short i = 0; i < count; i++) {
            Serial << "CONNECTION EVENT" << endl
                   << "Device:        "  << added[i]->productName << " | PID: " << added[i]->pid << endl  // TODO: Format as HEX
                   << "Manufacturer:  "  << added[i]->vendorName  << " | VID: " << added[i]->vid << endl
                   << "Adding `UsbDeviceInfo` to index." << endl << endl;
        }
        Serial << dec;
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
