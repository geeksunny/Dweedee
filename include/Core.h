#ifndef DWEEDEE_CORE_H
#define DWEEDEE_CORE_H

#include <Usb.h>

#include "Hotplug.h"
#include "UsbDeviceInfo.h"
#include "MidiDevice.h"


namespace dweedee {


    class Core : HotplugEventHandler {

        HotplugManager *usbMgr;
        std::deque<MidiDevice*> midiDevices_;

    public:
        explicit Core(USB *Usb);
        void task();
        void onDevicesAdded(UsbDeviceInfo **added, short count) override;
        void onDevicesRemoved(UsbDeviceInfo **removed, short count) override;

    };

}


#endif //DWEEDEE_CORE_H
