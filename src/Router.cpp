#include "Router.h"
#include "Utility.h"


namespace dweedee {

    Result::Result(bool consumed, bool failed) : consumed_(consumed), failed_(failed), msgCount_(0) {
        // todo
    }

    Result::Result(dweedee::MidiMessage *message) : consumed_(false), failed_(false), msgCount_(1) {
        // todo
    }

    Result::Result(dweedee::MidiMessage **messages, uint8_t msgCount) {
        // todo
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

    void Mapping::onMidiData(MidiDevice *device, MidiMessage *message) {
        //
    }

    Result Mapping::process() {
        // todo
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

    bool Mapping::addInput(dweedee::MidiDevice *inputDevice) {
        //
        if (activated_ && Router::getInstance() != nullptr) {
            Router::getInstance()->mapInputDevice(inputDevice, this);
        }
    }

    bool Mapping::addOutput(dweedee::MidiDevice *outputDevice) {
        // TODO:
        return false;
    }

    void Mapping::broadcast(dweedee::MidiMessage *message) {
        // single message
    }

    void Mapping::broadcast(dweedee::MidiMessage **messages, uint8_t msgCount) {
        // multiple messages
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
        //
    }

    bool Router::removeMapping(dweedee::Mapping *mapping) {
        // TODO: iterate through mappings.inputs_ and run removeInputMapping(input, mapping) on each
        // loop, true if found and removed, else false
    }

    bool Router::mapInputDevice(dweedee::MidiDevice *inputDevice, dweedee::Mapping *mapping) {
        auto pair = findInputMappingPair(inputDevice);
        if (pair != inputMappings_.end()) {
        }
        if (findInputMappingPair(inputDevice) == inputMappings_.end()) {
            // No registration exists; create one.
            std::pair<MidiDevice*, std::deque<Mapping*>> pair;
            pair.first = inputDevice;
            inputMappings_.push_back(pair);
        }
    }

    bool Router::removeInputMapping(dweedee::MidiDevice *inputDevice, dweedee::Mapping *mapping) {
        auto pair = findInputMappingPair(inputDevice);
        if (pair != inputMappings_.end()) {
        }
    }

    bool Router::deviceIsMapped(dweedee::MidiDevice *inputDevice) {
        return findInputMappingPair(inputDevice) != inputMappings_.end();
    }

    std::deque<InputMappingPair>::deque_iter Router::findInputMappingPair(dweedee::MidiDevice *device) {
        auto it = inputMappings_.begin();
        while (it != inputMappings_.end()) {
            if (it->first == device) {
                break;
            }
            ++it;
        }
        return it;
    }

    void Router::task() {
        if (!paused_) {
            // TODO: Iterate through live mappings, read, write, delete, repeat
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