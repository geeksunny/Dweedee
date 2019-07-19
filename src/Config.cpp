#include "Config.h"

namespace dweedee {

template <typename T>
void freeAndClear(std::deque<T *> &deque) {
  for (auto it = deque.begin(); it != deque.end(); /* it moved by .erase()*/) {
    free(*it);
    it = deque.erase(it);
  }
}

////////////////////////////////////////////////////////////////
// Base Classes : NamedDeque, NamedRecord  /////////////////////
////////////////////////////////////////////////////////////////

template <typename T>
NamedRecord<T>::NamedRecord(const char *name) : name_((char *) malloc(strlen(name)+1)) {
  strcpy(name_, name);
}

template<typename T>
NamedRecord<T>::~NamedRecord() {
  free(name_);
}

template <typename T>
bool NamedDeque<T>::matches(const char *&key, const T &value) {
  return STR_EQ(key, value.name_);
}

////////////////////////////////////////////////////////////////
// Class : Config  /////////////////////////////////////////////
////////////////////////////////////////////////////////////////

Config::~Config() {
  freeAndClear(ignore_);
}

void Config::onKey(const char *key, dweedee::JsonFileParser &parser) {
  // TODO: use PROG_STR here ?
  if (STR_EQ(key, "devices")) {
    parser.parse(devices_);
  } else if (STR_EQ(key, "ignore")) {
    parser.getStringArray(ignore_);
  } else if (STR_EQ(key, "mappings")) {
    parser.parse(mappings_);
  } else if (STR_EQ(key, "clock")) {
    parser.parse(clock_);
  } else if (STR_EQ(key, "sysex")) {
    parser.readArrayToBuffer();
    // TODO: Parse array of SysexRecord objects, store in sysex_
  } else if (STR_EQ(key, "options")) {
    parser.readObjectToBuffer();
    // TODO: Do stuff with buffer
  } else {
    // NOT FOUND
    // TODO: readObjectOrArray... ?
  }
}

////////////////////////////////////////////////////////////////
// Class : DevicesConfig  //////////////////////////////////////
////////////////////////////////////////////////////////////////

void DevicesConfig::onKey(const char *key, dweedee::JsonFileParser &parser) {
  this->push_back(DeviceRecord(key));
  parser.parse(this->back());
}

////////////////////////////////////////////////////////////////
// Class : DeviceRecord  ///////////////////////////////////////
////////////////////////////////////////////////////////////////

DeviceRecord::DeviceRecord(const char *name) : NamedRecord(name) {
  //
}

DeviceRecord::~DeviceRecord() {
  //
}

void DeviceRecord::onKey(const char *key, dweedee::JsonFileParser &parser) {
  if (STR_EQ(key, "vid")) {
    // Vendor ID
    parser.getHex(vid_);
  } else if (STR_EQ(key, "pid")) {
    // Product ID
    parser.getHex(pid_);
  }
}

////////////////////////////////////////////////////////////////
// Class : MappingConfig  //////////////////////////////////////
////////////////////////////////////////////////////////////////

void MappingConfig::onKey(const char *key, dweedee::JsonFileParser &parser) {
  this->push_back(MappingRecord(key));
  parser.parse(this->back());
}

////////////////////////////////////////////////////////////////
// Class : MappingRecord  //////////////////////////////////////
////////////////////////////////////////////////////////////////

MappingRecord::MappingRecord(const char *name) : NamedRecord(name) {
  //
}

MappingRecord::~MappingRecord() {
  // TODO: Delete other fields when implemented
  freeAndClear(inputs_);
  freeAndClear(outputs_);
}

void MappingRecord::onKey(const char *key, dweedee::JsonFileParser &parser) {
  if (STR_EQ(key, "inputs")) {
    // array of input nicknames
    parser.getStringArray(inputs_);
  } else if (STR_EQ(key, "outputs")) {
    // array of output nicknames
    parser.getStringArray(outputs_);
  } else if (STR_EQ(key, "filters")) {
    // NamedDeque of filter configurations
  } else if (STR_EQ(key, "listen")) {
    // listen to clock, sysex, activeSense messages? is this necessary?
  }
}

////////////////////////////////////////////////////////////////
// Class : ClockConfig  ////////////////////////////////////////
////////////////////////////////////////////////////////////////

void ClockConfig::onKey(const char *key, dweedee::JsonFileParser &parser) {
  if (STR_EQ(key, "inputs")) {
    // array of input nicknames
    parser.getStringArray(inputs_);
  } else if (STR_EQ(key, "outputs")) {
    // array of output nicknames
    parser.getStringArray(outputs_);
  } else if (STR_EQ(key, "bpm")) {
    // beats per minute / tempo
    parser.getInt(bpm_);
  } else if (STR_EQ(key, "ppqn")) {
    // pulses per quarter note
    parser.getInt(ppqn_);
  } else if (STR_EQ(key, "patternLength")) {
    // pattern length, in sixteenth notes
    parser.getInt(patternLength_);
  } else if (STR_EQ(key, "tapEnabled")) {
    parser.getBool(tapEnabled_);
  } else if (STR_EQ(key, "analog")) {
    // analog.volume ?
  }
}

////////////////////////////////////////////////////////////////
// Class : SysexRecord  ////////////////////////////////////////
////////////////////////////////////////////////////////////////

SysexRecord::~SysexRecord() {
  free(path_);
}

void SysexRecord::onKey(const char *key, dweedee::JsonFileParser &parser) {
  if (STR_EQ(key, "path")) {
    // File path
    parser.getString(path_);
  } else if (STR_EQ(key, "output")) {
    // Nickname of output device
  }
}

////////////////////////////////////////////////////////////////
// Functions  //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

Config parseConfigFromSd(const char *filename) {
  File file = Storage().open(filename);
  if (file.position() == (uint32_t) -1) {
    // file is an invalid stream, we cannot continue.
    return Config{};
  }
  JsonFileParser parser(file);
  Config config;
  // TODO : No dynamic cast needed here? dynamic_cast<dweedee::JsonModel &>(config)
  bool result = parser.parse(config);
  return config;
}

}
