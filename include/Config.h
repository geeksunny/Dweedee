#ifndef DWEEDEE_INCLUDE_CONFIG_H_
#define DWEEDEE_INCLUDE_CONFIG_H_

#include "Deques.h"
#include "Storage.h"

#define STR_EQ(StrA, StrB)  (strcmp(StrA, StrB) == 0)

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

// TODO: Keep a static index of nickname pointers to be shared between models

class DeviceRecord : public JsonModel, public NamedRecord<DeviceRecord> {

  friend class DevicesConfig;

  int vid_;
  int pid_;
  // TODO: should device manufacturer strings be allowed as alternative?

  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;
 public:
  DeviceRecord(const char *name);
  virtual ~DeviceRecord();

};

class DevicesConfig : public JsonModel, NamedDeque<DeviceRecord> {
  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;
};

class ListenConfig : public JsonModel {

  bool clock_;
  bool sysex_;
  bool activeSense_;

  void onKey(const char *key, JsonFileParser &parser) override;

};

class FilterRecord : public JsonModel, public NamedRecord<FilterRecord> {
  // STUB CLASS - TODO: Build this out
  void onKey(const char *key, JsonFileParser &parser) override;
};

class FiltersConfig : public JsonModel, public NamedDeque<FilterRecord> {
  // STUB CLASS - TODO: Build this out
  void onKey(const char *key, JsonFileParser &parser) override;
};

class MappingRecord : public JsonModel, public NamedRecord<MappingRecord> {

  friend class MappingConfig;

  std::deque<char *> inputs_;
  std::deque<char *> outputs_;
  FiltersConfig filters_;
  ListenConfig listen_;

  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;

 public:
  MappingRecord(const char *name);
  ~MappingRecord();

};

class MappingConfig : public JsonModel, NamedDeque<MappingRecord> {
  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;
};

class AnalogClockConfig : public JsonModel {

  int volume_;

  void onKey(const char *key, JsonFileParser &parser) override;

};

class ClockConfig : public JsonModel {

  std::deque<char *> inputs_;
  std::deque<char *> outputs_;
  int bpm_;
  int ppqn_;
  int patternLength_;
  bool tapEnabled_;
  AnalogClockConfig analog_;

  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;

};

class SysexRecord : public JsonModel {

  // TODO: Consider moving SysexRecord over to a NamedRecord<SysexRecord> in the future

  char *path_ = nullptr;
  char *output_ = nullptr;

  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;

 public:
  ~SysexRecord();

};

///////////////////////
// Base config class //

class Config : public JsonModel {

  DevicesConfig devices_;
  std::deque<char *> ignore_;
  MappingConfig mappings_;
  ClockConfig clock_;
  std::deque<SysexRecord> sysex_;

  void onKey(const char *key, JsonFileParser &parser) override;
//  void serialize() override;

 public:
//  Config() = default;
  ~Config();

};

Config parseConfigFromSd(const char *filename);

}

#endif //DWEEDEE_INCLUDE_CONFIG_H_
