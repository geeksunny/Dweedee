#ifndef DWEEDEE_ROUTER_H
#define DWEEDEE_ROUTER_H


#include <deque>
#include "MidiDevice.h"

namespace dweedee {

    class Result {

        bool consumed_;
        bool failed_;
        uint8_t msgCount;
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


    class Mapping : MidiReadHandler {

        std::deque<MidiDevice*> inputs_;
        std::deque<MidiDevice*> outputs_;
        // todo: filters
        bool activated_ = false;

        void onMidiData(MidiDevice *device, MidiMessage *message) override;

    public:
        Mapping();
//        ~Mapping();   // TODO!
        Result process();
        bool activate();
        bool deactivate();
        bool addInput(MidiDevice *inputDevice);
        bool addOutput(MidiDevice *outputDevice);
        void broadcast(MidiMessage *message);
        void broadcast(MidiMessage **messages, uint8_t msgCount);

    };


    class Router {

        friend class Mapping;

        static Router *instance;

        bool paused_ = false;
        std::deque<std::pair<MidiDevice*, std::deque<Mapping*>>> inputMappings_;
        std::deque<Mapping*> mappings_;

        bool addMapping(Mapping *mapping);
        bool removeMapping(Mapping *mapping);
        bool registerInputDevice(MidiDevice inputDevice);
        bool unregisterInputDevice(MidiDevice inputDevice);
        bool deviceIsMapped(MidiDevice inputDevice);

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
