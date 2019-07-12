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
  return strcmp(key, value.name_) == 0;
}

////////////////////////////////////////////////////////////////
// Class : Config  /////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void Config::onKey(const char *key, dweedee::JsonParser &parser) {
  // TODO: use PROG_STR here ?
  if (strcmp(key, "devices") == 0) {
    parser.readObjectToBuffer();
    // TODO: Do stuff with buffer
  } else if (strcmp(key, "ignore") == 0) {
    parser.readArrayToBuffer();
    // TODO: Do stuff with buffer
  } else if (strcmp(key, "mappings") == 0) {
    parser.readObjectToBuffer();
    // TODO: Do stuff with buffer
  } else if (strcmp(key, "clock") == 0) {
    parser.readObjectToBuffer();
    // TODO: Do stuff with buffer
  } else if (strcmp(key, "sysex") == 0) {
    parser.readObjectToBuffer();
    // TODO: Do stuff with buffer
  } else if (strcmp(key, "options") == 0) {
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

void DevicesConfig::onKey(const char *key, dweedee::JsonParser &parser) {
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

void DeviceRecord::onKey(const char *key, dweedee::JsonParser &parser) {
  if (strcmp(key, "vid") == 0) {
    //
  } else if (strcmp(key, "pid") == 0) {
    //
  } else if (strcmp(key, "name") == 0) {  // TODO: IS necessary ?
    //
  }
}

////////////////////////////////////////////////////////////////
// Class : MappingConfig  //////////////////////////////////////
////////////////////////////////////////////////////////////////

void MappingConfig::onKey(const char *key, dweedee::JsonParser &parser) {
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

void MappingRecord::onKey(const char *key, dweedee::JsonParser &parser) {
  if (strcmp(key, "inputs") == 0) {
    // array of input nicknames
  } else if (strcmp(key, "outputs") == 0) {
    // array of output nicknames
  } else if (strcmp(key, "filters") == 0) {
    // NamedDeque of filter configurations
  } else if (strcmp(key, "listen") == 0) {
    // listen to clock, sysex, activeSense messages? is this necessary?
  }
}

////////////////////////////////////////////////////////////////
// Class : ClockConfig  ////////////////////////////////////////
////////////////////////////////////////////////////////////////

void ClockConfig::onKey(const char *key, dweedee::JsonParser &parser) {
  if (strcmp(key, "inputs") == 0) {
    // array of input nicknames
  } else if (strcmp(key, "outputs") == 0) {
    // array of output nicknames
  } else if (strcmp(key, "bpm") == 0) {
    // beats per minute / tempo, 120
  } else if (strcmp(key, "ppqn") == 0) {
    // pulses per quarter note, 24
  } else if (strcmp(key, "patternLength") == 0) {
    // int, 16 (sixteenth notes)
  } else if (strcmp(key, "tapEnabled") == 0) {
    // true/false
  } else if (strcmp(key, "analog") == 0) {
    // analog.volume ?
  }
}

////////////////////////////////////////////////////////////////
// Class : SysexRecord  ////////////////////////////////////////
////////////////////////////////////////////////////////////////

void SysexRecord::onKey(const char *key, dweedee::JsonParser &parser) {
  if (strcmp(key, "path") == 0) {
    // File path
  } else if (strcmp(key, "output") == 0) {
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
  JsonParser parser(file);
  Config config;
  // TODO : No dynamic cast needed here? dynamic_cast<dweedee::JsonModel &>(config)
  bool result = parser.parse(config);
  return config;
}

}
