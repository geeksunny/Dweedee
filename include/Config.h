#ifndef DWEEDEE_INCLUDE_CONFIG_H_
#define DWEEDEE_INCLUDE_CONFIG_H_

#include "Deques.h"
#include "Storage.h"

namespace dweedee {

/////////////////////////
// Named Deque classes //

template <typename T>
class NamedDeque : public KeyedDeque<const char *, T> {
  bool matches(const char *&key, const T &value) override;
};

template <typename T>
class NamedRecord {
  friend class NamedDeque<T>;

  char *name_;

 public:
  NamedRecord(const char *name);
  ~NamedRecord();
};

///////////////////////
// Base config class //

class Config : public JsonModel {

  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;

 public:
//  Config() = default;

};

Config parseConfigFromSd(const char *filename);

}

#endif //DWEEDEE_INCLUDE_CONFIG_H_
