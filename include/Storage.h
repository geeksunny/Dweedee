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
#define KEY_LENGTH_MAX    24

namespace dweedee {

class JsonFileParser;

class JsonModel {

  friend class JsonFileParser;

  virtual void onKey(const char *key, JsonFileParser &parceler) = 0;
//  virtual void serialize() = 0;

};

class JsonFileParser {

  friend class JsonModel;

  DynamicJsonDocument buffer_;
  File &src_;
  char keyBuffer_[KEY_LENGTH_MAX + 1];

 public:
  explicit JsonFileParser(File &src);
  bool parse(JsonModel &dest);
  bool findNextKey(char *dest, int maxLength);
  bool readJsonToBuffer(char openChar);
  bool readObjectToBuffer();
  bool readArrayToBuffer();
  bool getString(char* dest);

  // TODO: See if this will ever work... should act like Java's <T extends JsonModel>
//  template <typename T>
//  typename JSON::enable_if<JSON::is_base_of<JsonModel, T>::value, bool>::type parse(T &dest);
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
