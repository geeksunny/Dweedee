#ifndef DWEEDEE_HOTPLUG_H
#define DWEEDEE_HOTPLUG_H

#include <ArduinoSTL.h>
#include <deque>
#include <usbh_midi.h>
#include <usbhub.h>

#include "UsbDeviceInfo.h"


namespace dweedee {


    class HotplugEventHandler {

    public:
        virtual void onDevicesAdded(UsbDeviceInfo **added, short count) = 0;
        virtual void onDevicesRemoved(UsbDeviceInfo **removed, short count) = 0;

    };


    class UsbDevicePool {

        std::deque<USBHub*> hubPool_;
        std::deque<USBH_MIDI*> midiPool_;

        bool requestUsbHub(USB *Usb);
        bool requestUsbMidi(USB *Usb);

    public:
        UsbDevicePool(USB *Usb);
        bool checkAllocation(USB *Usb);
        USBHub *getUsbHub(uint8_t devAddr);
        USBH_MIDI *getUsbMidi(uint8_t devAddr);
        uint8_t getActiveUsbHubCount();
        uint8_t getActiveUsbMidiCount();
        bool isIndexed(uint8_t devAddr);
        bool removeFromIndex(uint8_t devAddr);

    };


    class HotplugManager {

        static HotplugManager *instance;
        static UsbDevicePool *devicePool;
        static void checkUsbDevice(UsbDevice *pdev);
        static std::deque<UsbDeviceInfo*>::deque_iter findDevInfo(
                std::deque<UsbDeviceInfo*> *deque, uint8_t devAddress);

        USB *Usb_;
        unsigned short int devicesAdded_ = 0;
        std::deque<UsbDeviceInfo*> usbDeviceIndex_;
        std::deque<UsbDeviceInfo*> usbDeviceQueue_;
        HotplugEventHandler *eventHandler_;

        UsbDeviceInfo *createDeviceInfo(UsbDevice *pdev);
        char *getStringDescriptor(uint8_t usbDevAddr, uint8_t strIndex);
        void resetUsbDevAddrQueue();
        bool isConfigDescriptorUsbMidi(uint8_t devAddr, uint8_t configDescr);

    public:
        explicit HotplugManager(USB *Usb, HotplugEventHandler *eventHandler);
        void task();
        void processUsbDevice(UsbDevice *pdev);
        bool isUsbMidi(uint8_t devAddr);
        USB *getUsb() const;
        UsbDevicePool *getUsbDevicePool() const;

    };


}


#endif //DWEEDEE_HOTPLUG_H
