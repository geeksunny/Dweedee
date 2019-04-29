#include "Core.h"
#include <PrintStream.h>    // swap with <ArduinoSTL.h>

#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]




HotplugManager *HotplugManager::instance;

HotplugManager::HotplugManager(USB *Usb, HotplugEventHandler *eventHandler) : Usb_(Usb), eventHandler_(eventHandler) {
    if (HotplugManager::instance != nullptr) {
        // TODO: Throw exception
    }
    // TODO: Null checks?
    HotplugManager::instance = this;
    Hub_ = new USBHub((USB*)Usb);    // TODO: Should this be declared in top-most level?
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

std::deque<UsbDeviceInfo*>::deque_iter HotplugManager::findDevInfo(
        std::deque<UsbDeviceInfo*> *deque,
        uint8_t usbDevAddr) {

    auto it = deque->begin();
    while (it != deque->end()) {
        if ((*it)->devAddress == usbDevAddr) {
            break;
        }
    }
    return it;
}

void HotplugManager::task() {
    Usb_->Task();
    resetUsbDevAddrQueue();
    if (Usb_->getUsbTaskState() == USB_STATE_RUNNING) {
        Usb_->ForEachUsbDevice(&checkUsbDevice);
    }
    if (!usbDeviceQueue_.empty()) {
        auto it = usbDeviceQueue_.begin();
        while (it != usbDeviceQueue_.end()) {
            // TODO: Build arrays of added[] and removed[] for eventHandler callbacks

            Serial << "DISCONNECT EVENT" << endl
                   << "Device:        "  << (*it)->productName << " | PID: " << (*it)->pid << endl
                   << "Manufacturer:  "  << (*it)->vendorName  << " | VID: " << (*it)->vid << endl
                   << "Removing `UsbDeviceInfo` from index." << endl << endl;

            auto iit = findDevInfo(&usbDeviceIndex_, (*it)->devAddress);
            if (iit != usbDeviceIndex_.end()) {
                // TODO: ITEM WAS ALREADY INDEXED!! (Disconnect event... right?)
//                usbDeviceIndex_.erase(usbDeviceIndex_.find(it->devAddress));
            } else {
                // TODO: ITEM WAS NOT INDEXED, FRESH CONNECTION (New connection event)
            }

            it = usbDeviceQueue_.erase(it);
        }
    }
}

void HotplugManager::resetUsbDevAddrQueue() {
    devicesAdded_ = 0;
    for (auto it = usbDeviceIndex_.begin(); it != usbDeviceIndex_.end(); ++it) {
        usbDeviceQueue_.push_back(*it);
    }
}

void HotplugManager::checkUsbDevice(UsbDevice *pdev) {
    HotplugManager::instance->processUsbDevice(pdev);
}

void HotplugManager::processUsbDevice(UsbDevice *pdev) {
    UsbDevAddr id = pdev->address.devAddress;
    auto it = findDevInfo(&usbDeviceQueue_, id);
    if (it != usbDeviceIndex_.end()) {
        // The device is already indexed. Remove it from the processing queue.
        usbDeviceQueue_.erase(it);
        return;
    }
    // Parse usb device info from pdev into a UsbDeviceInfo and index in usbDeviceIndex_.
    UsbDeviceInfo *info = getDeviceInfo(pdev);
    usbDeviceQueue_.push_back(info);
    // TODO: add info to usbDeviceQueue; (move map indexing to eventHandler??)
    devicesAdded_++;

    Serial << "CONNECTION EVENT" << endl
           << "Device:        "  << info->productName << " | PID: " << info->pid << endl  // TODO: Format as HEX
           << "Manufacturer:  "  << info->vendorName  << " | VID: " << info->vid << endl
           << "Adding `UsbDeviceInfo` to index." << endl << endl;
}

UsbDeviceInfo* HotplugManager::getDeviceInfo(UsbDevice *pdev) {
    auto *result = new UsbDeviceInfo();

    USB_DEVICE_DESCRIPTOR deviceDescriptor;
    byte rcode;
    rcode = Usb_->getDevDescr(pdev->address.devAddress, 0, 0x12, (uint8_t *)&deviceDescriptor);
    if (rcode) {
        Serial.print("rcode [device descriptor] :: ");
        Serial.println(rcode, HEX);
    }
    result->devAddress = pdev->address.devAddress;
    result->vid = deviceDescriptor.idVendor;
    result->pid = deviceDescriptor.idProduct;
    result->vendorName = this->getStringDescriptor(pdev->address.devAddress, deviceDescriptor.iManufacturer);
    result->productName = this->getStringDescriptor(pdev->address.devAddress, deviceDescriptor.iProduct);
    // Device serial number at `deviceDescriptor.iSerial`

    return result;
}

char* HotplugManager::getStringDescriptor(byte usbDevAddr, byte strIndex) {
    uint8_t buf[66];
    byte rcode;
    byte length;
    unsigned int langid;

    rcode = Usb_->getStrDescr(usbDevAddr, 0, 1, 0, 0, buf);
    if (rcode) {
        Serial.print("Error retrieving LangID table length (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }
    length = buf[0];
    rcode = Usb_->getStrDescr(usbDevAddr, 0, length, 0, 0, buf);
    if (rcode) {
        Serial.print("Error retrieving LangID table (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }
    HIBYTE(langid) = buf[3];
    LOBYTE(langid) = buf[2];
    rcode = Usb_->getStrDescr(usbDevAddr, 0, 1, strIndex, langid, buf);
    if (rcode) {
        Serial.print("Error retrieving string length (rcode ");
        Serial.print(rcode, HEX);
        Serial.println(")");
        return (char *) "Unknown";  // todo: define constant
    }
    length = buf[0];
    rcode = Usb_->getStrDescr(usbDevAddr, 0, length, strIndex, langid, buf);
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


Core::Core(USB *Usb) : usbMgr(new HotplugManager(Usb, this)) {
    //
}

void Core::task() {
    usbMgr->task();
}

void Core::onDevicesAdded(UsbDeviceInfo added[]) {
    // TODO: Create / enable UsbMidiDevices with contents of added[]
}

void Core::onDevicesRemoved(UsbDeviceInfo removed[]) {
    // TODO: Remove / disable UsbMidiDevices matching contents of removed[]
}
