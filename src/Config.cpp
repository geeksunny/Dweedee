#include "Config.h"

namespace dweedee {

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

void Config::serialize() {

}

void Config::deserialize(JsonObject &object) {

}

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
