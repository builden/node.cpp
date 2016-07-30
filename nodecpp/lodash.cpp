#include "lodash.h"
#include "core-math.h"

namespace nodecpp {
  Lodash &_ = Lodash::instance();

  int Lodash::random(int lower /*= 0*/, int upper /*= 1*/) {
    if (lower > upper) {
      int tmp = lower;
      lower = upper;
      upper = tmp;
    }
    return lower + Math.floor(Math.random() * (upper - lower + 1));
  }

}