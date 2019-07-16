#include "Config.h"

namespace dweedee {

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

void Config::onKey(const char *key, dweedee::JsonFileParser &parser) {
  // TODO: use PROG_STR here ?
  if (STR_EQ(key, "devices")) {
    parser.parse(devices_);
  } else if (STR_EQ(key, "ignore")) {
    parser.readArrayToBuffer();
    // TODO: Do stuff with buffer
  } else if (STR_EQ(key, "mappings")) {
    parser.parse(mappings_);
  } else if (STR_EQ(key, "clock")) {
    parser.readObjectToBuffer();
    // TODO: Do stuff with buffer
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
  // TODO
}

DeviceRecord::~DeviceRecord() {
  // TODO: Delete any fields here when implemented
}

void DeviceRecord::onKey(const char *key, dweedee::JsonFileParser &parser) {
  if (STR_EQ(key, "vid")) {
    parser.getString(vid_);
  } else if (STR_EQ(key, "pid")) {
    parser.getString(pid_);
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
  // TODO: Init other fields when implemented
}

MappingRecord::~MappingRecord() {
  // TODO: Delete other fields when implemented
}

void MappingRecord::onKey(const char *key, dweedee::JsonFileParser &parser) {
  if (STR_EQ(key, "inputs")) {
    // array of input nicknames
  } else if (STR_EQ(key, "outputs")) {
    // array of output nicknames
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
  } else if (STR_EQ(key, "outputs")) {
    // array of output nicknames
  } else if (STR_EQ(key, "bpm")) {
    // beats per minute / tempo, 120
  } else if (STR_EQ(key, "ppqn")) {
    // pulses per quarter note, 24
  } else if (STR_EQ(key, "patternLength")) {
    // int, 16 (sixteenth notes)
  } else if (STR_EQ(key, "tapEnabled")) {
    // true/false
  } else if (STR_EQ(key, "analog")) {
    // analog.volume ?
  }
}

////////////////////////////////////////////////////////////////
// Class : SysexRecord  ////////////////////////////////////////
////////////////////////////////////////////////////////////////

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
  if (file.position() == -1) {  // TODO: Will this work for validity check?
    // file is an invalid stream, we cannot continue.
    // TODO: Return Config{} ?
  }
  JsonFileParser parser(file);
  Config config;
  // TODO : No dynamic cast needed here? dynamic_cast<dweedee::JsonModel &>(config)
  bool result = parser.parse(config);
  return config;
}

}
