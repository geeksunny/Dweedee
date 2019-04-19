#ifndef DWEEDEE_CORE_H
#define DWEEDEE_CORE_H

#include <Arduino.h>

#include <SPI.h>
#include <Usb.h>
#include <usbhub.h>

#include <ArduinoSTL.h>
#include <vector>
#include <map>

#include "UsbDeviceInfo.h"


typedef uint8_t UsbDevAddr;


class HotplugManager {

    USB *Usb;
    USBHub *Hub;
    std::map<UsbDevAddr, UsbDeviceInfo> usbDeviceMap;
    std::vector<UsbDevAddr> usbDeviceQueue;

    UsbDeviceInfo getDeviceInfo(UsbDevice *pdev);
    char* getStringDescriptor(byte usbDevAddr, byte strIndex);
    void resetUsbDevAddrQueue();

public:
    explicit HotplugManager(USB *Usb);
    void task();
    void processUsbDevice(UsbDevice *pdev);

};


class Core {

    HotplugManager *usbMgr;

public:
    explicit Core(USB *Usb);
    void task();

};


#endif //DWEEDEE_CORE_H
