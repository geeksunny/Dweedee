#include "Core.h"
#include <PrintStream.h>    // swap with <ArduinoSTL.h>

#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]


Core* inst;

void checkUsbDevice(UsbDevice *pdev) {
    inst->processUsbDevice(pdev);
}


Core::Core(USB *Usb) : Usb(Usb) {
    inst = this;
    Hub = new USBHub((USB*)Usb);    // TODO: Should this be declared in top-most level?
    Serial.begin(9600);
    Serial.println("Starting USB interface.");
    if (Usb->Init() == -1) {
        Serial.println("USB Host Shield did not start. Halting.");
        while (1); //halt
    }
    Serial.println("Started USB interface.");
    delay(200);
    Serial.println("Ready.");
}

void Core::task() {
    Usb->Task();
    if (Usb->getUsbTaskState() == USB_STATE_RUNNING) {
        resetUsbDevAddrQueue();
        Usb->ForEachUsbDevice(&checkUsbDevice);
        if (!usbDeviceQueue.empty()) {
            Serial.println("Processing Device Info queue.");
            auto it = usbDeviceQueue.begin();
            while (it != usbDeviceQueue.end()) {
                MidiDeviceInfo *info = &usbDeviceMap[(UsbDevAddr) it];
                Serial << "DISCONNECT EVENT" << endl
                       << "Device:        "  << info->productName << " | PID: " << info->pid << endl
                       << "Manufacturer:  "  << info->vendorName  << " | VID: " << info->vid << endl
                       << "Adding `MidiDeviceInfo` to index." << endl << endl;

                usbDeviceMap.erase(usbDeviceMap.find((UsbDevAddr) it));
                it = usbDeviceQueue.erase(it);
            }
        }
    }
}

void Core::resetUsbDevAddrQueue() {
    for (auto it = usbDeviceMap.begin(); it != usbDeviceMap.end(); ++it) {
        usbDeviceQueue.push_back(it->first);
    }
}

void Core::processUsbDevice(UsbDevice *pdev) {
    UsbDevAddr id = pdev->address.devAddress;
    if (usbDeviceMap.count(id) == 1) {
        // The device is already indexed. Remove it from the processing queue.
        usbDeviceQueue.erase(std::remove(usbDeviceQueue.begin(), usbDeviceQueue.end(), id), usbDeviceQueue.end());
        return;
    }
    // Parse usb device info from pdev into a MidiDeviceInfo and index in usbDeviceMap.
    MidiDeviceInfo info = getDeviceInfo(pdev);
    usbDeviceMap[id] = info;

    Serial << "CONNECTION EVENT" << endl
           << "Device:        "  << info.productName << " | PID: " << info.pid << endl
           << "Manufacturer:  "  << info.vendorName  << " | VID: " << info.vid << endl
           << "Adding `MidiDeviceInfo` to index." << endl << endl;
}

MidiDeviceInfo Core::getDeviceInfo(UsbDevice *pdev) {
    MidiDeviceInfo result = MidiDeviceInfo();

    USB_DEVICE_DESCRIPTOR deviceDescriptor;
    byte rcode;
    rcode = Usb->getDevDescr(pdev->address.devAddress, 0, 0x12, (uint8_t *)&deviceDescriptor);
    if (rcode) {
        Serial.print("rcode [device descriptor] :: ");
        Serial.println(rcode, HEX);
    }
    result.devAddress = pdev->address.devAddress;
    result.vid = deviceDescriptor.idVendor;
    result.pid = deviceDescriptor.idProduct;
    result.vendorName = this->getStringDescriptor(pdev->address.devAddress, deviceDescriptor.iManufacturer);
    result.productName = this->getStringDescriptor(pdev->address.devAddress, deviceDescriptor.iProduct);
    // Device serial number at `deviceDescriptor.iSerial`

    return result;
}

char* Core::getStringDescriptor(byte usbDevAddr, byte strIndex) {
    uint8_t buf[66];
    byte rcode;
    byte length;
    unsigned int langid;

    rcode = Usb->getStrDescr(usbDevAddr, 0, 1, 0, 0, buf);
    if (rcode) {
        Serial.print("Error retrieving LangID table length (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }
    length = buf[0];
    rcode = Usb->getStrDescr(usbDevAddr, 0, length, 0, 0, buf);
    if (rcode) {
        Serial.print("Error retrieving LangID table (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }
    HIBYTE(langid) = buf[3];
    LOBYTE(langid) = buf[2];
    rcode = Usb->getStrDescr(usbDevAddr, 0, 1, strIndex, langid, buf);
    if (rcode) {
        Serial.print("Error retrieving string length (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }
    length = buf[0];
    rcode = Usb->getStrDescr(usbDevAddr, 0, length, strIndex, langid, buf);
    if (rcode) {
        Serial.print("Error retrieving string (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }

    char *result = (char *) malloc(((length - 3) / 2) + 1);
    byte i;
    int bufIdx = 0;
    for (i = 2; i < length; i += 2) {
        result[bufIdx++] = (char) buf[i];
    }
    result[bufIdx] = '\0';

    return result;
}