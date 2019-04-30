#ifndef DWEEDEE_CORE_H
#define DWEEDEE_CORE_H

#include <Arduino.h>

#include <SPI.h>
#include <Usb.h>
#include <usbhub.h>

#include <ArduinoSTL.h>
#include <deque>

#include "UsbDeviceInfo.h"


typedef uint8_t UsbDevAddr;


class HotplugEventHandler {

public:
    virtual void onDevicesAdded(UsbDeviceInfo *added[], short count) = 0;
    virtual void onDevicesRemoved(UsbDeviceInfo *removed[], short count) = 0;

};


class HotplugManager {

    static HotplugManager *instance;
    static void checkUsbDevice(UsbDevice *pdev);

    USB *Usb_;
    USBHub *Hub_;
    unsigned short int devicesAdded_ = 0;
    std::deque<UsbDeviceInfo*> usbDeviceIndex_;
    std::deque<UsbDeviceInfo*> usbDeviceQueue_;
    HotplugEventHandler *eventHandler_;

    UsbDeviceInfo* getDeviceInfo(UsbDevice *pdev);
    char* getStringDescriptor(byte usbDevAddr, byte strIndex);
    void resetUsbDevAddrQueue();
    std::deque<UsbDeviceInfo*>::deque_iter findDevInfo(std::deque<UsbDeviceInfo*> *deque, uint8_t usbDevAddr);

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
    void onDevicesAdded(UsbDeviceInfo *added[], short count) override;
    void onDevicesRemoved(UsbDeviceInfo *removed[], short count) override;

};


#endif //DWEEDEE_CORE_H
