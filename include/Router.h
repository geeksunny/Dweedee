#ifndef DWEEDEE_ROUTER_H
#define DWEEDEE_ROUTER_H

#include <queue>
#include "MidiDevice.h"
#include "Filter.h"

namespace dweedee {

class Mapping {

  friend class Result;
  friend class InputMapping;
  friend class Router;

  static std::queue<MidiMessage *> processQueueA;
  static std::queue<MidiMessage *> processQueueB;
  static std::queue<MidiMessage *> *inputQueue;
  static std::queue<MidiMessage *> *outputQueue;

  std::deque<MidiDevice *> inputs_;
  std::deque<MidiDevice *> outputs_;
  std::deque<Filter *> filters_;
  bool activated_ = false;

  void broadcast(MidiMessage *message);
  void broadcast(MidiMessage **messages, uint8_t msgCount);
  bool process(MidiMessage *message);

 public:
  Mapping();
  bool activate();
  bool deactivate();
  bool isActivated();
  bool addInput(MidiDevice *inputDevice);
  bool removeInput(MidiDevice *inputDevice);
  bool addOutput(MidiDevice *outputDevice);
  bool removeOutput(MidiDevice *outputDevice);
  bool addFilter(Filter *filter);
  bool removeFilter(Filter *filter);

};

class InputMapping : MidiReadHandler {

  friend class Router;

  MidiDevice *device_;
  std::deque<Mapping *> mappings_;

  void onMidiData(MidiDevice *device, MidiMessage *message) override;
  void process();

 public:
  bool operator==(const MidiDevice *rhs) const;
  bool operator!=(const MidiDevice *rhs) const;

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
  std::deque<Mapping *> mappings_;

  bool addMapping(Mapping *mapping);
  bool removeMapping(Mapping *mapping);
  bool mapInputDevice(MidiDevice *inputDevice, Mapping *mapping);
  bool removeInputMapping(MidiDevice *inputDevice, Mapping *mapping);
  bool deviceIsMapped(MidiDevice *inputDevice);

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
