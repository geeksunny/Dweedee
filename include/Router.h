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
        Result(MidiMessage *message);
        Result(MidiMessage *messages[], uint8_t msgCount);
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
        bool enabled_ = true;   // todo : should this default to false?

        void onMidiData(MidiDevice *device, MidiMessage *message) override;

    public:
        Mapping();
        Result process();
        bool addInput(MidiDevice *inputDevice);
        bool addOutput(MidiDevice *outputDevice);
        void broadcast(MidiMessage *message);
        void broadcast(MidiMessage **messages, uint8_t msgCount);

    };


    class Router {

        static Router *instance;

        bool paused_ = false;
        std::deque<Mapping> mappings_;

    public:
        static Router *getInstance();

        Router();
        void task();
        void setPaused(bool paused);
        void toggle();
        bool isPaused();

    };

}


#endif //DWEEDEE_ROUTER_H
