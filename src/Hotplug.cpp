#include "Hotplug.h"
#include "Utility.h"
#include <PrintStream.h>    // swap with <ArduinoSTL.h>


#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]


namespace dweedee {

    HotplugManager *HotplugManager::instance;

    HotplugManager::HotplugManager(USB *Usb, HotplugEventHandler *eventHandler) : Usb_(Usb), eventHandler_(eventHandler) {
        if (HotplugManager::instance != nullptr) {
            // TODO: Throw exception
        }
        // TODO: Null checks on Usb, eventHandler ?
        HotplugManager::instance = this;
        Serial.begin(9600);
        Serial.println("Starting USB interface.");
        if (Usb->Init() == -1) {
            Serial.println("USB Host Shield did not start. Halting.");
            haltBlinking(1000);
        }
        Serial.println("Started USB interface.");
        delay(200);
        Serial.println("Ready.");
    }

    std::deque<UsbDeviceInfo*>::deque_iter HotplugManager::findDevInfo(
            std::deque<UsbDeviceInfo*> *deque,
            uint8_t usbDevAddr) {

        auto it = deque->begin();
        while (it < deque->end()) {
            if ((*it)->bmAddress == usbDevAddr) {
                break;
            }
            it++;
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
            // added[]'s size is this->devicesAdded_;
            int remSize = usbDeviceQueue_.size() - devicesAdded_;
            UsbDeviceInfo *added[devicesAdded_];
            UsbDeviceInfo *removed[remSize];
            short addIdx = 0;
            short remIdx = 0;

            auto it = usbDeviceQueue_.begin();
            while (it < usbDeviceQueue_.end()) {
                auto indexPos = findDevInfo(&usbDeviceIndex_, (*it)->bmAddress);
                if (indexPos < usbDeviceIndex_.end()) {
                    // Device disconnection event
                    if (remIdx < remSize) {
                        removed[remIdx++] = *it;
                    } else {
                        // Tried to set within `removed[]` but index is out of range.
                        // TODO: Handle error? Should this prevent code past this if/else from running? Halt operation?
                        // TODO --- ENDLESS LOOP HERE AND TEST ---
                    }
                    usbDeviceIndex_.erase(indexPos);
                } else {
                    // Device connection event
                    if (addIdx < devicesAdded_) {
                        added[addIdx++] = *it;
                        usbDeviceIndex_.push_back(*it);
                    } else {
                        // Tried to access `added[]` but index is out of range.
                        // TODO: Handle error? Should this prevent code past this if/else from running? Halt operation?
                        // TODO --- ENDLESS LOOP HERE AND TEST ---
                    }
                }
                // Move the iterator by erasing the current item
                it = usbDeviceQueue_.erase(it);
            }

            if (eventHandler_ != nullptr) {
                if (addIdx > 0) {
                    eventHandler_->onDevicesAdded(added, addIdx);
                }
                if (remIdx > 0) {
                    eventHandler_->onDevicesRemoved(removed, remIdx);
                }
            } else {
                // Event handler is not set.
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
        uint8_t id = pdev->address.bmAddress;
        auto it = findDevInfo(&usbDeviceIndex_, id);
        if (it != usbDeviceIndex_.end()) {
            // The device is already indexed. Remove it from the processing queue.
            usbDeviceQueue_.erase(findDevInfo(&usbDeviceQueue_, id));
            return;
        }
        // Parse usb device info from pdev into a UsbDeviceInfo and index in usbDeviceIndex_.
        UsbDeviceInfo *info = getDeviceInfo(pdev);
        usbDeviceQueue_.push_back(info);
        devicesAdded_++;
    }

    UsbDeviceInfo* HotplugManager::getDeviceInfo(UsbDevice *pdev) {
        USB_DEVICE_DESCRIPTOR deviceDescriptor;
        byte rcode;
        rcode = Usb_->getDevDescr(pdev->address.bmAddress, 0, 0x12, (uint8_t *)&deviceDescriptor);
        if (rcode) {
            Serial.print("rcode [device descriptor] :: ");
            Serial.println(rcode, HEX);
        }

        if (deviceDescriptor.bDeviceClass == 0x09) {
            // * * NEW hub detected! * *
            Serial << " + + + HUB DETECTED + + + REGISTERING USBHub* + + +" << endl;
            usbHubs_.push_back(new USBHub(Usb_));
            Usb_->Task();
        }

        auto *result = new UsbDeviceInfo();

        result->bmAddress = pdev->address.bmAddress;
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

}