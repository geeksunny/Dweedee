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
        Usb->ForEachUsbDevice(&checkUsbDevice);
        if (!this->usbDeviceQueue.empty()) {
            Serial.println("Processing Device Info queue.");
            auto it = usbDeviceQueue.begin();
            while (it != usbDeviceQueue.end()) {
                Serial << "Device:        " << it->productName << " | PID: " << it->pid << endl;
                Serial << "Manufacturer:  " << it->vendorName << " | VID: " << it->vid << endl << endl;
                usbDeviceMap[it->devAddress] = true;
                it = usbDeviceQueue.erase(it);
            }
        }
    }
}

void Core::processUsbDevice(UsbDevice *pdev) {
    uint8_t id = pdev->address.devAddress;
    // TODO : This only considers newly connected devices; Need code to monitor for disconnects.
    if (usbDeviceMap.count(id) != 0) {
        return;
    }
    // Parse usb device info from pdev into a MidiDeviceInfo and store in usbDeviceQueue.
    MidiDeviceInfo info = getDeviceInfo(pdev);
    Serial.println("Adding `MidiDeviceInfo` to queue.");
    usbDeviceQueue.push_back(info);
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