#ifndef DWEEDEE_STORAGE_H
#define DWEEDEE_STORAGE_H

#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <avr/pgmspace.h>
#include <WString.h>
#include <deque>

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
  bool findArray();
  bool getBool(bool &dest);
  bool getHexString(int &dest);
  bool getInt(int &dest);
  bool getString(char *dest);
  bool getStringArray(std::deque<char *> &dest);

  bool peekUntil(char until, bool escape = false);
  bool readUntil(char until, char *dest, bool escape = false);

  // Included in header file due to compilation errors related to templating.
  template <typename T>
  bool getObjectArray(std::deque<T> &dest) {
    if (findArray()) {
      bool result;
      do {
        T obj;
        result = parse(obj);
        if (result) {
          dest.push_back(obj);
        }
      } while (result);
      // verify closing bracket
      int srcStartPos = src_.position();
      while (src_.available()) {
        if (src_.read() == ']') {
          return true;
        }
      }
      src_.seek(srcStartPos);
    }
    return false;
  }

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
