#include "Deques.h"

namespace dweedee {

template<typename K, typename T>
T &KeyedDeque<K, T>::find(const K &key) {
  for (auto it = this->begin(); it != this->end(); ++it) {
    if (matches(key, it)) {
      return it;
    }
  }
  return this->end();
}

}