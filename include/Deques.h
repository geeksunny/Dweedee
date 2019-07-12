#ifndef DWEEDEE_SRC_DEQUES_H_
#define DWEEDEE_SRC_DEQUES_H_

#include <deque>

namespace dweedee {

/**
 * Abstract deque with definable keyed lookup method.
 *
 * @tparam K    Key typename
 * @tparam T    Deque content typename
 */
template <typename K, typename T>
class KeyedDeque : public std::deque<T> {

  virtual bool matches(K &key, const T &value) = 0;

 public:
  T &find(const K &key);

};

}

#endif //DWEEDEE_SRC_DEQUES_H_
