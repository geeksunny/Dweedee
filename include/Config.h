#ifndef DWEEDEE_INCLUDE_CONFIG_H_
#define DWEEDEE_INCLUDE_CONFIG_H_

#include "Storage.h"

namespace dweedee {

class Config : public JsonModel {

  void onKey(const char *key, JsonParser &parser) override;
  void serialize() override;
  void deserialize(JsonObject &object) override;

 public:
//  Config() = default;

};

Config parseConfigFromSd(const char *filename);

}

#endif //DWEEDEE_INCLUDE_CONFIG_H_
