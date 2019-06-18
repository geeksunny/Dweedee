#include <Router.h>

namespace dweedee {

Filter::Filter() {
  //
}

bool Filter::isPaused() {
  return paused_;
}

void Filter::setPaused(bool isPaused) {
  paused_ = isPaused;
}

}