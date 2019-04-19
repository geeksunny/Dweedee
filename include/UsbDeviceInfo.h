#ifndef DWEEDEE_USBDEVICEINFO_H
#define DWEEDEE_USBDEVICEINFO_H


struct UsbDeviceInfo {
    uint8_t devAddress;
    uint16_t vid;
    uint16_t pid;
    char* vendorName;
    char* productName;
};


#endif //DWEEDEE_USBDEVICEINFO_H
