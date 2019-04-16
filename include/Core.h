#ifndef DWEEDEE_CORE_H
#define DWEEDEE_CORE_H

#include <Arduino.h>

#include <SPI.h>
#include <Usb.h>
#include <usbhub.h>

#include <ArduinoSTL.h>
#include <vector>
#include <map>

#include "MidiDevice.h"


class Core {

    USB *Usb;
    USBHub *Hub;
    std::map<uint8_t, bool> usbDeviceMap;
    std::vector<MidiDeviceInfo> usbDeviceQueue;

    MidiDeviceInfo getDeviceInfo(UsbDevice *pdev);
    byte getStringDescriptor(byte usbDevAddr, byte strIndex, char* bufPtr);

public:
    Core(USB *Usb);
    void task();
    void processUsbDevice(UsbDevice *pdev);

};


#endif //DWEEDEE_CORE_H
