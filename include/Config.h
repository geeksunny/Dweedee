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
// Config subclasses //

class DeviceRecord : public JsonModel, public NamedRecord<DeviceRecord> {

  friend class DevicesConfig;

  char *vid_{};
  char *pid_{};
  // TODO: should device manufacturer strings be allowed as alternative?

  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;
 public:
  DeviceRecord(const char *name);
  virtual ~DeviceRecord();
};

class DevicesConfig : public JsonModel, NamedDeque<DeviceRecord> {
  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;
};

class MappingRecord : public JsonModel, public NamedRecord<MappingRecord> {

  friend class MappingConfig;

  // TODO: Add other fields
 public:
  MappingRecord(const char *name);
  ~MappingRecord();

  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;
};

class MappingConfig : public JsonModel, NamedDeque<MappingRecord> {
  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;
};

class ClockConfig : public JsonModel {
  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;
};

class SysexRecord : public JsonModel {
  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;
};

///////////////////////
// Base config class //

class Config : public JsonModel {

  DevicesConfig devices;
  MappingConfig mappings;
  std::deque<SysexRecord> sysex;

  void onKey(const char *key, JsonParser &parser) override;
//  void serialize() override;

 public:
//  Config() = default;

};

Config parseConfigFromSd(const char *filename);

}

#endif //DWEEDEE_INCLUDE_CONFIG_H_
