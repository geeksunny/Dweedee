#include "Storage.h"

namespace dweedee {

JsonParceler::JsonParceler(Stream &src) : buffer_(DynamicJsonDocument(JSON_BUFFER_SIZE)), src_(src) {
  //
}
template<typename T>
typename JSON::enable_if<!JSON::is_base_of<JsonParceler, T>::value,
                         bool>::type JsonParceler::parse(T &dest) {
  // TODO: parse here
  return false;
}

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
