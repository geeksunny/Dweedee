#include <Arduino.h>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include <SPI.h>
#include <Usb.h>
#include <usbhub.h>

#include <ArduinoSTL.h>
#include <vector>
#include <map>

#include "MidiDevice.h"

#ifndef DWEEDEE_CORE_H
#define DWEEDEE_CORE_H


class Core {

    USB Usb;
    USBHub Hub = USBHub(&Usb);
    std::map<uint8_t, bool> usbDeviceMap;
    std::vector<MidiDeviceInfo> usbDeviceQueue;

    MidiDeviceInfo getDeviceInfo(UsbDevice *pdev);
    byte getStringDescriptor(byte usbDevAddr, byte strIndex, char* bufPtr);

public:
    Core();
    void task();
    void processUsbDevice(UsbDevice *pdev);

};


#endif //DWEEDEE_CORE_H
