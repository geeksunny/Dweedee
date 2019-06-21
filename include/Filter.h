#ifndef DWEEDEE_FILTER_H
#define DWEEDEE_FILTER_H

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
//  ~Result();    // TODO!
  bool isConsumed();
  bool isFailed();
  bool shouldBroadcast();
  MidiMessage *getMessage();
  MidiMessage **getMessages();
  uint8_t getMessageCount();

};

class Filter {

  bool paused_ = false;

 public:
  Filter();
  bool isPaused();
  void setPaused(bool isPaused);

  virtual Result process(MidiMessage *message) = 0;

};

class ChannelFilter : Filter {
 public:
  ChannelFilter();
  Result process(MidiMessage *message) override;
};

class ChordFilter : Filter {
 public:
  ChordFilter();
  Result process(MidiMessage *message) override;
};

class DelayFilter : Filter {
 public:
  DelayFilter();
  Result process(MidiMessage *message) override;
};

class MessageTypeFilter : Filter {
 public:
  MessageTypeFilter();
  Result process(MidiMessage *message) override;
};

class TransposeFilter : Filter {
 public:
  TransposeFilter();
  Result process(MidiMessage *message) override;
};

class VelocityFilter : Filter {
 public:
  VelocityFilter();
  Result process(MidiMessage *message) override;
};

}

#endif //DWEEDEE_FILTER_H
