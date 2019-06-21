#include "Router.h"

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
  return messages_;
}

uint8_t Result::getMessageCount() {
  return msgCount_;
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