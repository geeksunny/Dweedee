#include "Storage.h"

namespace dweedee {

////////////////////////////////////////////////////////////////
// Class : JsonFileParser  /////////////////////////////////////
////////////////////////////////////////////////////////////////

JsonFileParser::JsonFileParser(File &src) : buffer_(DynamicJsonDocument(JSON_BUFFER_SIZE)), src_(src), keyBuffer_{} {
  //
}

bool JsonFileParser::parse(dweedee::JsonModel &dest) {
  while (findNextKey(keyBuffer_, KEY_LENGTH_MAX)) {
    Serial.print("Processing object for key : ");
    Serial.println(keyBuffer_);
    dest.onKey(keyBuffer_, *this);
  }
  return false;
}

bool JsonFileParser::findNextKey(char *dest, int maxLength) {
  if (!src_.find('"')) {
    return false;
  }
  char c;
  bool ignoreNext = false;
  for (int i = 0; i < maxLength; ++i) {
    c = src_.read();
    if (c == '\\' && !ignoreNext) {
      ignoreNext = true;
      dest[i] = c;
      continue;
    }
    if (c == '"' && !ignoreNext) {
      dest[i] = '\0';
      return true;
    } else {
      dest[i] = c;
      ignoreNext = false;
    }
  }
  // Reached end of maxLength before setting null terminator. Setting it here.
  dest[maxLength] = '\0';
  return true;
}

bool JsonFileParser::readJsonToBuffer(const char openChar) {
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

bool JsonFileParser::readObjectToBuffer() {
  return readJsonToBuffer('{');
}

bool JsonFileParser::readArrayToBuffer() {
  return readJsonToBuffer('[');
}

bool JsonFileParser::findArray() {
  int srcStartPos = src_.position();
  while (src_.available()) {
    switch (src_.read()) {
      case '[':
        return true;
      case ']':
      case '{':
      case ',':
        goto ARRAY_NOT_FOUND;
    }
  }
  ARRAY_NOT_FOUND:
  src_.seek(srcStartPos);
  return false;
}

bool JsonFileParser::getBool(bool &dest) {
  // TODO: Look for characters (true/false) OR digit (0/1)
  return false;
}

bool JsonFileParser::getHex(int &dest) {
  // TODO: Look for digits, bail on invalid char (return 0? return -1?)
  //  use <sstream>
  return false;
}

bool JsonFileParser::getInt(int &dest) {
  // TODO: Look for digits, bail on invalid char (return 0? return -1?)
  return false;
}

bool JsonFileParser::getString(char *dest) {
  int srcStartPos = src_.position();
  int strPos = -1;
  int strLen = 0;
  bool ignoreNext = false;
  // Find string, get length. Bail if we think the next value is not a string.
  while (src_.available()) {
    if (ignoreNext) {
      src_.read();
      ++strLen;
      ignoreNext = false;
      continue;
    }
    switch (src_.peek()) {
      case '.':
      case ',':
      case '{':
      case '[':
      case '}':
      case ']':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        // Invalid character - bail!
        src_.seek(srcStartPos);
        return false;
      case '"':
        if (strPos == -1) {
          // Beginning of string
          src_.read();
          strPos = src_.position();
        } else {
          // End of string
          goto STRING_LOOP;
        }
        break;
      case '\\':
        // Escape character - skip inspection of next character
        ignoreNext = true;
        // Fall through
      default:
        src_.read();
        if (strPos != -1) {
          ++strLen;
        }
        break;
    }
  }
  STRING_LOOP:
  if (strPos > -1) {
    // Store the string in dest pointer
    dest = (char *) malloc(strLen + 1);
    src_.seek(strPos);
    for (strPos = 0; strPos < strLen; ++strPos) {
      dest[strPos] = src_.read();
    }
    dest[strPos] = '\0';
    return true;
  }
  // String not found; Reset stream position
  src_.seek(srcStartPos);
  return false;
}

bool JsonFileParser::getStringArray(std::deque<char *> &dest) {
  // approach opening bracket
  if (findArray()) {
    // get string until false result
    bool result;
    do {
      char *str;
      result = getString(str);
      if (result) {
        dest.push_back(str);
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

////////////////////////////////////////////////////////////////
// Class : Storage  ////////////////////////////////////////////
////////////////////////////////////////////////////////////////

Storage::Storage() {
  // todo ?
}

File Storage::open(const char *filename) {
  if (!SD.begin(SD_CS_PIN)) { // TODO: call SD.end() when finished with the file stream? (yes I think so)
    return File{};
  }
  return SD.open(filename);
}

}
