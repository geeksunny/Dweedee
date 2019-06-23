#include "Router.h"

// Queue macros
#define CLEAR(Queue)            while (!Queue->empty()) { Queue->pop(); }
// Deque macros
#define FIND(Iterable, Value)   (std::find(Iterable.begin(), Iterable.end(), Value))
#define HAS(Iterable, Value)    (std::find(Iterable.begin(), Iterable.end(), Value) != Iterable.end())

namespace dweedee {

std::queue<MidiMessage *> Mapping::processQueueA;
std::queue<MidiMessage *> Mapping::processQueueB;
std::queue<MidiMessage *> *Mapping::inputQueue = &Mapping::processQueueA;
std::queue<MidiMessage *> *Mapping::outputQueue = &Mapping::processQueueB;

Mapping::Mapping() {
  //
}

void Mapping::broadcast(dweedee::MidiMessage *message) {
  for (auto output = outputs_.begin(); output != outputs_.end(); ++output) {
    (*output)->write(message);
  }
}

void Mapping::broadcast(dweedee::MidiMessage **messages, uint8_t msgCount) {
  for (int i = 0; i < msgCount; ++i) {
    for (auto output = outputs_.begin(); output != outputs_.end(); ++output) {
      (*output)->write(messages[i]);
    }
  }
}

bool Mapping::process(MidiMessage *message) {
  // When this method returns, inputQueue & outputQueue must both be empty for the next time this method is executed.
  if (filters_.empty()) {
    broadcast(message);
    return false;
  }
  inputQueue->push(new MidiMessage(*message));
  for (auto filter = filters_.begin(); filter != filters_.end(); ++filter) {
    while (!inputQueue->empty()) {
      Result processed = (*filter)->process(inputQueue->front());
      inputQueue->pop();
      if (processed.isConsumed() || processed.isFailed()) {
        CLEAR(inputQueue)
        CLEAR(outputQueue)
        return processed.isConsumed();
      }
    }
    std::swap(inputQueue, outputQueue);
  }
  // Broadcast messages to outputs. Using inputQueue due to pointer swap at end of filter loop.
  while (!inputQueue->empty()) {
    for (auto output = outputs_.begin(); output != outputs_.end(); ++output) {
      (*output)->write(inputQueue->front());
    }
    delete inputQueue->front();
    inputQueue->pop();
  }
  return false;
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

bool Mapping::removeInput(dweedee::MidiDevice *inputDevice) {
  auto it = FIND(inputs_, inputDevice);
  if (it != inputs_.end()) {
    inputs_.erase(it);
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

bool Mapping::removeOutput(dweedee::MidiDevice *outputDevice) {
  auto it = FIND(outputs_, outputDevice);
  if (it != outputs_.end()) {
    outputs_.erase(it);
    return true;
  }
  return false;
}

bool Mapping::addFilter(dweedee::Filter *filter) {
  if (!HAS(filters_, filter)) {
    filters_.push_back(filter);
    return true;
  }
  return false;
}

bool Mapping::removeFilter(dweedee::Filter *filter) {
  auto it = FIND(filters_, filter);
  if (it != filters_.end()) {
    filters_.erase(it);
    return true;
  }
  return false;
}

InputMapping::InputMapping(MidiDevice *inputDevice) : device_(inputDevice) {
  //
}

bool InputMapping::operator==(const MidiDevice *rhs) const {
  return device_ == rhs;
}

bool InputMapping::operator!=(const MidiDevice *rhs) const {
  return device_ != rhs;
}

void InputMapping::onMidiData(MidiDevice *device, MidiMessage *message) {
  for (auto mapping = mappings_.begin(); mapping != mappings_.end(); ++mapping) {
    bool consumed = (*mapping)->process(message);
    if (consumed) {
      break;
    }
  }
  delete message;
}

void InputMapping::process() {
  device_->read(*this);
}

bool InputMapping::add(dweedee::Mapping *mapping) {
  if (!HAS(mappings_, mapping)) {
    mappings_.push_back(mapping);
    return true;
  }
  return false;
}

bool InputMapping::remove(dweedee::Mapping *mapping) {
  auto it = FIND(mappings_, mapping);
  if (it != mappings_.end()) {
    mappings_.erase(it);
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
  if (HAS(mappings_, mapping) || mapping->isActivated()) {
    // TODO: Should the check to mapping->isActivated() prevent success here?
    return false;
  }
  mappings_.push_back(mapping);
  for (auto it = mapping->inputs_.begin(); it != mapping->inputs_.end(); ++it) {
    mapInputDevice((*it), mapping);
  }
  return true;
}

bool Router::removeMapping(dweedee::Mapping *mapping) {
  auto mappingPos = FIND(mappings_, mapping);
  if (mappingPos == mappings_.end()) {
    return false;
  }
  for (auto input = mapping->inputs_.begin(); input != mapping->inputs_.end(); ++input) {
    removeInputMapping(*input, mapping);
  }
  mappings_.erase(mappingPos);
  return false;
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
      inputMappings_.erase(inputMapping);
    }
    return result;
  }
  return false;
}

bool Router::deviceIsMapped(dweedee::MidiDevice *inputDevice) {
  return HAS(inputMappings_, inputDevice);
}

void Router::task() {
  if (paused_) {
    return;
  }
  for (auto it = inputMappings_.begin(); it != inputMappings_.end(); ++it) {
    (*it).process();
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