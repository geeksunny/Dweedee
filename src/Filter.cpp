#include "Router.h"

namespace dweedee {

Result::Result(bool consumed, bool failed) : consumed_(consumed), failed_(failed) {
  //
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

void Result::addMessage(MidiMessage *message) {
  Mapping::outputQueue->push(message);
}

Filter::Filter() {
  //
}

bool Filter::isPaused() {
  return paused_;
}

void Filter::setPaused(bool isPaused) {
  paused_ = isPaused;
}

}