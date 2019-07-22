#include "Storage.h"
#include <cctype>
#include <sstream>

namespace dweedee {

////////////////////////////////////////////////////////////////
// Class : JsonFileParser  /////////////////////////////////////
////////////////////////////////////////////////////////////////

JsonFileParser::JsonFileParser(File &src) : src_(src), keyBuffer_{} {
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
  int srcStartPos = src_.position();
  bool isStr = false;
  char next;
  while (src_.available()) {
    next = src_.peek();
    switch (next) {
      case 't':
      case 'T':
        dest = readMatches("true", false);
        goto BOOL_END;
      case 'f':
      case 'F':
        dest = readMatches("false", false);
        goto BOOL_END;
      case '0':
        dest = false;
        src_.read();
        goto BOOL_END;
      case '1':
        dest = true;
        src_.read();
        goto BOOL_END;
      case '"':
        isStr = true;
        break;
      default:
        if (!isspace(next) && next != ':') {
          // Invalid character found before value encountered, error result.
          src_.seek(srcStartPos);
          return false;
        }
    }
    src_.read();
  }
  BOOL_END:
  if (isStr) {
    if (src_.read() != '"') {
      // Did not find the string closure, error result.
      src_.seek(srcStartPos);
      return false;
    }
  }
  return true;
}

bool JsonFileParser::getHexString(int &dest) {
  char *hexStr = nullptr;
  if (getString(hexStr)) {
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> dest;
    free(hexStr);
    return true;
  }
  return false;
}

bool JsonFileParser::getInt(int &dest) {
  int srcStartPos = src_.position();
  char next;
  while (src_.available()) {
    next = src_.peek();
    if (isdigit(next)) {
      dest = src_.parseInt();
      // Skip to the end of the value, in case a float value is being processed.
      if (skipValue()) {
        return true;
      } else {
        // Encountered an error with value skip, error result.
        break;
      }
    } else if (!isspace(next) && next != ':') {
      // Invalid character found before value encountered, error result.
      break;
    }
  }
  // Didn't find a valid integer value, error result.
  src_.seek(srcStartPos);
  return false;
}

bool JsonFileParser::getString(char *dest) {
  int srcStartPos = src_.position();
  char next;
  bool success = false;
  // Find string, get length. Bail if we think the next value is not a string.
  while (src_.available()) {
    next = src_.read();
    if (next == '"') {
      success = readUntil('"', dest, true);
      break;
    } else if (!isspace(next) && next != ':') {
      break;
    }
  }
  if (!success) {
    // String not found; Reset stream position
    src_.seek(srcStartPos);
  }
  return success;
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

bool JsonFileParser::peekUntil(char until, bool escape) {
  int srcStartPos = src_.position();
  bool isEscaped = false;
  char next;
  while (src_.available()) {
    if (isEscaped) {
      src_.read();
      isEscaped = false;
      continue;
    }
    next = src_.peek();
    if (escape && next == '\\') {
      isEscaped = true;
    } else if (next == until) {
      return true;
    }
    src_.read();
  }
  src_.seek(srcStartPos);
  return false;
}

bool JsonFileParser::readUntil(char until, char *dest, bool escape) {
  int srcStartPos = src_.position();
  bool isEscaped = false;
  int strLen = 0;
  char next;
  // Read ahead to get string length.
  while (src_.available()) {
    next = src_.read();
    if (isEscaped) {
      isEscaped = false;
    } else if (escape && next == '\\') {
      isEscaped = true;
    } else if (next == until) {
      break;
    }
    ++strLen;
  }
  // Consider a zero-length string an error result.
  if (strLen == 0) {
    src_.seek(srcStartPos);
    return false;
  }
  // Read string to dest pointer.
  src_.seek(srcStartPos);
  dest = (char *) malloc(strLen + 1);
  for (int i = 0; i < strLen; ++i) {
    dest[i] = src_.read();
  }
  dest[strLen] = '\0';
  return true;
}

bool JsonFileParser::readMatches(const char *value, const bool caseSensitive) {
  int srcStartPos = src_.position();
  char srcNext, valueNext;
  for (int i = 0; value[i] != '\0'; ++i) {
    if (caseSensitive) {
      srcNext = src_.read();
      valueNext = value[i];
    } else {
      srcNext = tolower(src_.read());
      valueNext = tolower(value[i]);
    }
    if (srcNext != valueNext) {
      src_.seek(srcStartPos);
      return false;
    }
  }
  return true;
}

bool JsonFileParser::skipValue() {
  while (src_.available()) {
    switch (src_.read()) {
      case ',':
      case ']':
      case '}':
        return true;
    }
  }
  // Reached the end of the file... error result ?
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
