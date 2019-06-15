#ifndef DWEEDEE_ROUTER_H
#define DWEEDEE_ROUTER_H


#include <deque>
#include "MidiDevice.h"

namespace dweedee {

    class Result {

        bool consumed_;
        bool failed_;
        uint8_t msgCount_;
        MidiMessage *messages_[];

    public:
        Result(bool consumed, bool failed);
        explicit Result(MidiMessage *message);
        Result(MidiMessage *messages[], uint8_t msgCount);
//        ~Result();    // TODO!
        bool isConsumed();
        bool isFailed();
        bool shouldBroadcast();
        MidiMessage *getMessage();
        MidiMessage **getMessages();
        uint8_t getMessageCount();

    };


    class Mapping {

        friend class Router;

        std::deque<MidiDevice*> inputs_;
        std::deque<MidiDevice*> outputs_;
        // todo: filters
        bool activated_ = false;

    public:
        Mapping();
//        ~Mapping();   // TODO!
        Result process(MidiMessage *message);
        bool activate();
        bool deactivate();
        bool isActivated();
        bool addInput(MidiDevice *inputDevice);
//        bool removeInput(MidiDevice *inputDevice);    // TODO
        bool addOutput(MidiDevice *outputDevice);
//        bool removeOutput(MidiDevice *outputDevice);  // TODO
        void broadcast(MidiMessage *message);
        void broadcast(MidiMessage **messages, uint8_t msgCount);

    };


    class InputMapping : MidiReadHandler {

        friend class Router;

        MidiDevice *device_;
        std::deque<Mapping*> mappings_;

        void onMidiData(MidiDevice *device, MidiMessage *message) override;

    public:
        bool operator==(const MidiDevice &rhs) const;
        bool operator!=(const MidiDevice &rhs) const;

        InputMapping(MidiDevice *inputDevice);
        bool add(Mapping *mapping);
        bool remove(Mapping *mapping);
        bool isEmpty();

    };


    class Router {

        friend class Mapping;

        static Router *instance;

        bool paused_ = false;
        std::deque<InputMapping> inputMappings_;
        std::deque<Mapping*> mappings_;

        bool addMapping(Mapping *mapping);
        bool removeMapping(Mapping *mapping);
        bool mapInputDevice(MidiDevice *inputDevice, Mapping *mapping);
        bool removeInputMapping(MidiDevice *inputDevice, Mapping *mapping);
        bool deviceIsMapped(MidiDevice *inputDevice);

    public:
        static Router *getInstance();

        Router();
//        ~Router();    // TODO!
        void task();
        void setPaused(bool paused);
        void toggle();
        bool isPaused();

    };

}


#endif //DWEEDEE_ROUTER_H
