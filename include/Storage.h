#ifndef DWEEDEE_STORAGE_H
#define DWEEDEE_STORAGE_H

#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <avr/pgmspace.h>
#include <WString.h>

#define JSON ARDUINOJSON_NAMESPACE

// Flash string macros
#define PROG_STR(Name, Content)   const char PROGMEM Name[] = Content
#define FCAST(Name)               reinterpret_cast<const __FlashStringHelper*>(Name)

#define SD_CS_PIN         53 // Uno: 4, 10 | Mega: 53
#define JSON_BUFFER_SIZE  1024

namespace dweedee {

class JsonParcel {
 public:
  virtual void serialize() = 0;
  virtual void deserialize(JsonObject &object) = 0;
};

class JsonParceler {
  DynamicJsonDocument buffer_;
  Stream &src_;
 public:
  JsonParceler(Stream &src);

  template<typename T>
  typename JSON::enable_if<!JSON::is_base_of<JsonParceler, T>::value, bool>::type parse(T &dest);
};

// TODO : Add support for storing binary compressed(?) version of the configuration in EEPROM.
class Storage {
//  File root;
 public:
  Storage();
  File open(const char *filename);
};

}

#endif //DWEEDEE_STORAGE_H
