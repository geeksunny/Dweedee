#include "Hotplug.h"
#include "Utility.h"
#include <PrintStream.h>    // swap with <ArduinoSTL.h>


#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]

#define DESC_BUFF_SIZE                  256
#define UNKNOWN                         "Unknown"
#define USB_SUBCLASS_MIDISTREAMING      3


namespace dweedee {

    HotplugManager *HotplugManager::instance;

    HotplugManager::HotplugManager(USB *Usb, HotplugEventHandler *eventHandler) : Usb_(Usb), eventHandler_(eventHandler) {
        if (HotplugManager::instance != nullptr) {
            // TODO: Throw exception
        }
        // TODO: Null checks on Usb, eventHandler ?
        HotplugManager::instance = this;
        usbHubs_.push_back(new USBHub(Usb_));
        Serial.begin(9600);
        Serial.println("Starting USB interface.");
        if (Usb->Init() == -1) {
            Serial.println("USB Host Shield did not start. Halting.");
            haltBlinking(1000);
        }
        Serial.println("Started USB interface.");
        delay(200);
        Serial << "Ready." << endl << uppercase << showbase;
    }

    std::deque<UsbDeviceInfo*>::deque_iter HotplugManager::findDevInfo(
            std::deque<UsbDeviceInfo*> *deque,
            uint8_t devAddress) {

        auto it = deque->begin();
        while (it < deque->end()) {
            if ((*it)->devAddress == devAddress) {
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
                auto indexPos = findDevInfo(&usbDeviceIndex_, (*it)->devAddress);
                if (indexPos < usbDeviceIndex_.end()) {
                    // Device disconnection event
                    if (remIdx < remSize) {
                        removed[remIdx++] = *it;

                        if ((*it)->devClass == USB_CLASS_HUB) {
                            --hubsActive_;
                            // TODO: Can / should we be deleting unneeded USBHub objects? That would be done here.
                        }
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
                        // If hub, increment hubActive_ and maybe create new USBHub object.
                        if ((*it)->devClass == USB_CLASS_HUB) {
                            // Ensure a free USBHub is always available for a potential hub connection.
                            // usbHubs_.size() should always be hubsActive_+1
                            if (++hubsActive_ == usbHubs_.size()) {
                                usbHubs_.push_back(new USBHub(Usb_));
                            }
                        }
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
            // Deleting unplugged UsbDeviceInfo objects.
            if (remIdx > 0) {
                for (short i = 0; i < remIdx; ++i) {
                    delete removed[i];
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
        auto it = findDevInfo(&usbDeviceIndex_, pdev->address.devAddress);
        if (it != usbDeviceIndex_.end()) {
            // The device is already indexed. Remove it from the processing queue.
            usbDeviceQueue_.erase(findDevInfo(&usbDeviceQueue_, pdev->address.devAddress));
            return;
        }
        // Parse usb device info from pdev into a UsbDeviceInfo and index in usbDeviceIndex_.
        UsbDeviceInfo *info = createDeviceInfo(pdev);
        usbDeviceQueue_.push_back(info);
        ++devicesAdded_;
    }

    UsbDeviceInfo* HotplugManager::createDeviceInfo(UsbDevice *pdev) {
        USB_DEVICE_DESCRIPTOR deviceDescriptor;
        byte rcode;
        rcode = Usb_->getDevDescr(pdev->address.devAddress, 0, 0x12, (uint8_t *)&deviceDescriptor);
//        if (rcode) {
//            Serial << "rcode [device descriptor] :: " << hex << rcode << dec << endl;
//        }

        auto *result = new UsbDeviceInfo();
        result->devAddress = pdev->address.devAddress;

        if (rcode) {
            result->vid = 0xdead;
            result->pid = 0xbeef;
            result->vendorName = (char *) UNKNOWN;
            result->productName = (char *) UNKNOWN;
        } else {
            result->devClass = deviceDescriptor.bDeviceClass;
            result->vid = deviceDescriptor.idVendor;
            result->pid = deviceDescriptor.idProduct;
            result->vendorName = this->getStringDescriptor(pdev->address.devAddress, deviceDescriptor.iManufacturer);
            result->productName = this->getStringDescriptor(pdev->address.devAddress, deviceDescriptor.iProduct);
        }
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
            Serial << "Error retrieving LangID table length ( rcode " << hex << rcode << dec << " ) " << endl;
            return (char *) UNKNOWN;
        }
        length = buf[0];
        rcode = Usb_->getStrDescr(usbDevAddr, 0, length, 0, 0, buf);
        if (rcode) {
            Serial << "Error retrieving LangID table ( rcode " << hex << rcode << dec << " ) " << endl;
            return (char *) UNKNOWN;
        }
        HIBYTE(langid) = buf[3];
        LOBYTE(langid) = buf[2];
        rcode = Usb_->getStrDescr(usbDevAddr, 0, 1, strIndex, langid, buf);
        if (rcode) {
            Serial << "Error retrieving string length (rcode " << hex << rcode << dec << ")" << endl;
            return (char *) UNKNOWN;
        }
        length = buf[0];
        rcode = Usb_->getStrDescr(usbDevAddr, 0, length, strIndex, langid, buf);
        if (rcode) {
            Serial << "Error retrieving string (rcode " << hex << rcode << dec << ")" << endl;
            return (char *) UNKNOWN;
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

    bool HotplugManager::isUsbMidi(uint8_t devAddr) {
        // Iterate through descriptors, return true as soon as we find a UsbMidi device descriptor.
        USB_DEVICE_DESCRIPTOR buf;
        uint8_t rcode;

        rcode = Usb_->getDevDescr(devAddr, 0, 0x12, (uint8_t *)&buf);
        if (rcode) {
            // Can't retrieve a device descriptor
            return false;
        }

        for (int i = 0; i < buf.bNumConfigurations; ++i) {
            if (isConfigDescriptorUsbMidi(devAddr, i)) {
                return true;
            }
        }
        return false;
    }

    bool HotplugManager::isConfigDescriptorUsbMidi(uint8_t devAddr, uint8_t configDescr) {
        uint8_t buf[DESC_BUFF_SIZE];
        uint8_t *buf_ptr = buf;
        uint8_t rcode;
        uint8_t descr_length;
        uint8_t descr_type;
        uint16_t total_length;

        // get config descr size
        rcode = Usb_->getConfDescr(devAddr, 0, 4, configDescr, buf);
        if (rcode) {
            // No size found
            return false;
        }
        total_length = buf[2] | ((int)buf[3] << 8);
        if (total_length > DESC_BUFF_SIZE) {    // check if total length is larger than buffer
            total_length = DESC_BUFF_SIZE;
        }

        // get whole configuration descriptor
        rcode = Usb_->getConfDescr(devAddr, 0, total_length, configDescr, buf);
        if (rcode) {
            // Invalid response
            return false;
        }

        // Parsing descriptors
        while (buf_ptr < (buf + total_length)) {
            descr_length = *(buf_ptr);
            descr_type = *(buf_ptr + 1);
            if (descr_type == USB_DESCRIPTOR_INTERFACE) {
                if (buf_ptr[5] == USB_CLASS_AUDIO && buf_ptr[6] == USB_SUBCLASS_MIDISTREAMING) {
                    return true;
                }
            }
            // Advance buffer pointer
            buf_ptr += descr_length;
        }
        return false;
    }

    USB *HotplugManager::getUsb() const {
        return Usb_;
    }

}