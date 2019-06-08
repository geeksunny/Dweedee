#include "Router.h"


namespace dweedee {

    Result::Result(bool consumed, bool failed) {
        // todo
    }

    Result::Result(dweedee::MidiMessage *message) {
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
        //
    }

    MidiMessage **Result::getMessages() {
        //
    }

    uint8_t Result::getMessageCount() {
        //
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
        //
    }

    bool Mapping::deactivate() {
        //
    }

    bool Mapping::addInput(dweedee::MidiDevice *inputDevice) {
        //
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


    Router *Router::instance;

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
        //
    }

    bool Router::registerInputDevice(dweedee::MidiDevice inputDevice) {
        //
    }

    bool Router::unregisterInputDevice(dweedee::MidiDevice inputDevice) {
        //
    }

    bool Router::deviceIsMapped(dweedee::MidiDevice inputDevice) {
        //
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