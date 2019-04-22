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


class HotplugEventHandler {

public:
    virtual void onDevicesAdded(UsbDeviceInfo added[]) = 0;
    virtual void onDevicesRemoved(UsbDeviceInfo removed[]) = 0;

};


class HotplugManager {

    static HotplugManager *instance;
    static void checkUsbDevice(UsbDevice *pdev);

    USB *Usb_;
    USBHub *Hub_;
    unsigned short int devicesAdded_ = 0;
    std::map<UsbDevAddr, UsbDeviceInfo> usbDeviceMap_;
    std::vector<UsbDevAddr> usbDeviceQueue_;
    HotplugEventHandler *eventHandler_;

    UsbDeviceInfo getDeviceInfo(UsbDevice *pdev);
    char* getStringDescriptor(byte usbDevAddr, byte strIndex);
    void resetUsbDevAddrQueue();

public:
    explicit HotplugManager(USB *Usb, HotplugEventHandler *eventHandler);
    void task();
    void processUsbDevice(UsbDevice *pdev);

};


class Core : HotplugEventHandler {

    HotplugManager *usbMgr;

public:
    explicit Core(USB *Usb);
    void task();
    void onDevicesAdded(UsbDeviceInfo added[]) override;
    void onDevicesRemoved(UsbDeviceInfo removed[]) override;

};


#endif //DWEEDEE_CORE_H
