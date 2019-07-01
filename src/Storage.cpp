#include "Storage.h"

namespace dweedee {

////////////////////////////////////////////////////////////////
// Class : JsonParser  ///////////////////////////////////////
////////////////////////////////////////////////////////////////

JsonParser::JsonParser(Stream &src) : buffer_(DynamicJsonDocument(JSON_BUFFER_SIZE)), src_(src), keyBuffer_{} {
  //
}

bool JsonParser::parse(dweedee::JsonModel &dest) {
  while (findNextKey(keyBuffer_)) {
    Serial.print("Processing object for key : ");
    Serial.println(keyBuffer_);
    dest.onKey(keyBuffer_, *this);
  }
  return false;
}

bool JsonParser::findNextKey(char *dest) {
  if (!src_.find('"')) {
    return false;
  }
  char c;
  for (int i = 0; i < KEY_LENGTH_MAX; ++i) {
    c = src_.read();
    if (c == '"') {
      dest[i] = '\0';
      return true;
    } else {
      dest[i] = c;
    }
  }
  // Reached end of KEY_LENGTH_MAX before setting null terminator. Setting it here.
  dest[KEY_LENGTH_MAX] = '\0';
  return true;
}

bool JsonParser::readJsonToBuffer(const char openChar) {
  char c;
  while (src_.available()) {
    c = src_.peek();
    if (c == openChar) {
      DeserializationError result = deserializeJson(buffer_, src_);
      return (result.code() == DeserializationError::Ok);
    }
    src_.read();
  }
  return false;
}

bool JsonParser::readObjectToBuffer() {
  return readJsonToBuffer('{');
}

bool JsonParser::readArrayToBuffer() {
  return readJsonToBuffer('[');
}

////////////////////////////////////////////////////////////////
// Class : Storage  ////////////////////////////////////////////
////////////////////////////////////////////////////////////////

Storage::Storage() {
  // todo ?
}

File Storage::open(const char *filename) {
  if (!SD.begin(SD_CS_PIN)) {
    return File{};
  }
  return SD.open(filename);
}

}
