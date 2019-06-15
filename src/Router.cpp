#include "Router.h"


#define HAS(Iterable, Value)    (std::find(Iterable.begin(), Iterable.end(), Value) != Iterable.end())
#define FIND(Iterable, Value)   (std::find(Iterable.begin(), Iterable.end(), Value))


namespace dweedee {

    Result::Result(bool consumed, bool failed) : consumed_(consumed), failed_(failed), msgCount_(0) {
        // todo - anything else needed here??
    }

    Result::Result(dweedee::MidiMessage *message) : consumed_(false), failed_(false), msgCount_(1) {
        // todo
        // TODO: malloc for messages_
        messages_[0] = message;
    }

    Result::Result(dweedee::MidiMessage **messages, uint8_t msgCount) {
        // todo
        // TODO: malloc for messages_
    }

    bool Result::isConsumed() {
        return consumed_;
    }

    bool Result::isFailed() {
        return failed_;
    }

    bool Result::shouldBroadcast() {
        return !failed_ && !consumed_;
    }

    MidiMessage *Result::getMessage() {
        return (msgCount_ == 0) ? nullptr : messages_[0];
    }

    MidiMessage **Result::getMessages() {
        //
        return messages_;
    }

    uint8_t Result::getMessageCount() {
        return msgCount_;
    }


    Mapping::Mapping() {
        // todo
    }

    Result Mapping::process(MidiMessage *message) {
        // TODO: Run through mapping's filters when implementing
        return Result(message);
    }

    bool Mapping::activate() {
        if (activated_ || Router::getInstance() == nullptr) {
            return false;
        }
        activated_ = Router::getInstance()->addMapping(this);
        return activated_;
    }

    bool Mapping::deactivate() {
        if (!activated_ || Router::getInstance() == nullptr) {
            return false;
        }
        activated_ = !Router::getInstance()->removeMapping(this);
        return activated_;
    }

    bool Mapping::isActivated() {
        return activated_;
    }

    bool Mapping::addInput(dweedee::MidiDevice *inputDevice) {
        if (!HAS(inputs_, inputDevice)) {
            inputs_.push_back(inputDevice);
            if (activated_ && Router::getInstance() != nullptr) {
                Router::getInstance()->mapInputDevice(inputDevice, this);
            }
            return true;
        }
        return false;
    }

    bool Mapping::addOutput(dweedee::MidiDevice *outputDevice) {
        if (!HAS(outputs_, outputDevice)) {
            outputs_.push_back(outputDevice);
            return true;
        }
        return false;
    }

    void Mapping::broadcast(dweedee::MidiMessage *message) {
        // single message
        // TODO: loop through this->outputs_, .write(message) to all in deque.
    }

    void Mapping::broadcast(dweedee::MidiMessage **messages, uint8_t msgCount) {
        // multiple messages
        // TODO: loop through this->outputs_, loop messages and .write(message) to all in deque.
    }


    InputMapping::InputMapping(MidiDevice *inputDevice) : device_(inputDevice) {
        //
    }

    bool InputMapping::operator==(const MidiDevice &rhs) const {
        return device_ == &rhs;
    }

    bool InputMapping::operator!=(const MidiDevice &rhs) const {
        return device_ != &rhs;
    }

    void InputMapping::onMidiData(MidiDevice *device, MidiMessage *message) {
        for (auto it = mappings_.begin(); it != mappings_.end(); ++it) {
            // TODO: send message through Mapping.process(),
            //  determine if loop should end early or proceed to the next mapping
        }
    }

    bool InputMapping::add(dweedee::Mapping *mapping) {
        if (!HAS(mappings_, mapping)) {
            mappings_.push_back(mapping);
            return true;
        }
        return false;
    }

    bool InputMapping::remove(dweedee::Mapping *mapping) {
        if (HAS(mappings_, mapping)) {  // TODO: Replace with own call to std::find for usage in deletion
            // todo : remove from mapping
            return true;
        }
        return false;
    }

    bool InputMapping::isEmpty() {
        return mappings_.empty();
    }


    Router *Router::instance = nullptr;

    Router *Router::getInstance() {
        return instance;
    }

    Router::Router() {
        if (Router::instance != nullptr) {
            // TODO: error?
        }
        Router::instance = this;
    }

    bool Router::addMapping(dweedee::Mapping *mapping) {
        // if Router::getInstance()->hasMapping(this) return false
        if (HAS(mappings_, mapping) || mapping->isActivated()) {
            return false;
        }
        // TODO: iterate through mappings.inputs_ and run mapInputDevice(input, mapping) on each
        mappings_.push_back(mapping);
        for (auto it = mapping->inputs_.begin(); it != mapping->inputs_.end(); ++it) {
            mapInputDevice((*it), mapping);
        }
    }

    bool Router::removeMapping(dweedee::Mapping *mapping) {
        // TODO: iterate through mappings.inputs_ and run removeInputMapping(input, mapping) on each
        // loop, true if found and removed, else false
    }

    bool Router::mapInputDevice(dweedee::MidiDevice *inputDevice, dweedee::Mapping *mapping) {
        auto inputMapping = FIND(inputMappings_, inputDevice);
        if (inputMapping != inputMappings_.end()) {
            return inputMapping->add(mapping);
        }
        return false;
    }

    bool Router::removeInputMapping(dweedee::MidiDevice *inputDevice, dweedee::Mapping *mapping) {
        auto inputMapping = FIND(inputMappings_, inputDevice);
        if (inputMapping != inputMappings_.end()) {
            bool result = inputMapping->remove(mapping);
            if (inputMapping->isEmpty()) {
                // TODO: remove inputMapping entirely, it is no longer needed.
                //  ~remove from inputMappings_
                //  ~delete inputMapping
            }
            return result;
        }
        return false;
    }

    bool Router::deviceIsMapped(dweedee::MidiDevice *inputDevice) {
        return HAS(mappings_, inputDevice);
    }

    void Router::task() {
        if (paused_) {
            return;
        }
        for (auto it = inputMappings_.begin(); it != inputMappings_.end(); ++it) {
            // TODO: Iterate through live mappings, read, write, delete, repeat
            //  - read message from input
            //  - loop through mappings
            //  - check result of mapping->process()
            //  - if should broadcast, send to broadcast()
            //  - delete the MidiMessage/Result objects!
            //  - continue to next input; repeat;
        }
    }

    void Router::setPaused(bool paused) {
        paused_ = paused;
    }

    void Router::toggle() {
        paused_ = !paused_;
    }

    bool Router::isPaused() {
        return paused_;
    }

}