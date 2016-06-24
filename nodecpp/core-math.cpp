#include "core-math.h"
#include <math.h>

namespace nodecpp {

  int CoreMath::abs(int x) {
    return x > 0 ? x : -x;
  }

  int CoreMath::ceil(double x) {
    return (int)::ceil(x);
  }

  int CoreMath::floor(double x) {
    return (int)::floor(x);
  }

  int CoreMath::round(double x) {
    return (x > 0.0) ? floor(x + 0.5) : ceil(x - 0.5);
  }

  CoreMath &Math = CoreMath::instance();
}
