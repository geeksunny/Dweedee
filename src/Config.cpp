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
