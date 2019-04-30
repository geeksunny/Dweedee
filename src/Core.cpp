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
        UsbDeviceInfo *added[devicesAdded_];
        UsbDeviceInfo *removed[usbDeviceQueue_.size() - devicesAdded_];
        short addIdx = 0;
        short remIdx = 0;

        auto it = usbDeviceQueue_.begin();
        while (it != usbDeviceQueue_.end()) {
            auto iit = findDevInfo(&usbDeviceIndex_, (*it)->devAddress);
            if (iit != usbDeviceIndex_.end()) {
                // Device disconnection event
                removed[remIdx++] = *iit;
                usbDeviceIndex_.erase(findDevInfo(&usbDeviceIndex_, (*it)->devAddress));
            } else {
                // Device connection event
                added[addIdx++] = *iit;
                usbDeviceIndex_.push_back(*it);
            }

            it = usbDeviceQueue_.erase(it);
        }

        if (addIdx > 0) {
            if (eventHandler_ != nullptr) {
                eventHandler_->onDevicesAdded(added, addIdx);
            }
        }
        if (remIdx > 0) {
            if (eventHandler_ != nullptr) {
                eventHandler_->onDevicesRemoved(removed, remIdx);
            }
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
    devicesAdded_++;
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

void Core::onDevicesAdded(UsbDeviceInfo *added[], short count) {
    // TODO: Create / enable UsbMidiDevices with contents of added[]
    for (short i = 0; i <= count; i++) {
        Serial << "CONNECTION EVENT" << endl
               << "Device:        "  << added[i]->productName << " | PID: " << added[i]->pid << endl  // TODO: Format as HEX
               << "Manufacturer:  "  << added[i]->vendorName  << " | VID: " << added[i]->vid << endl
               << "Adding `UsbDeviceInfo` to index." << endl << endl;
    }
}

void Core::onDevicesRemoved(UsbDeviceInfo *removed[], short count) {
    // TODO: Remove / disable UsbMidiDevices matching contents of removed[]
    for (short i = 0; i <= count; i++) {
        Serial << "DISCONNECT EVENT" << endl
               << "Device:        "  << removed[i]->productName << " | PID: " << removed[i]->pid << endl
               << "Manufacturer:  "  << removed[i]->vendorName  << " | VID: " << removed[i]->vid << endl
               << "Removing `UsbDeviceInfo` from index." << endl << endl;
    }
}
