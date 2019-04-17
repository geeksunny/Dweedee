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


typedef uint8_t UsbDevAddr;


class Core {

    USB *Usb;
    USBHub *Hub;
    std::map<UsbDevAddr, MidiDeviceInfo> usbDeviceMap;
    std::vector<UsbDevAddr> usbDeviceQueue;

    MidiDeviceInfo getDeviceInfo(UsbDevice *pdev);
    char* getStringDescriptor(byte usbDevAddr, byte strIndex);
    void resetUsbDevAddrQueue();

public:
    explicit Core(USB *Usb);
    void task();
    void processUsbDevice(UsbDevice *pdev);

};


#endif //DWEEDEE_CORE_H
