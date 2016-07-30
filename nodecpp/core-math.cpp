#include "core-math.h"
#include "nodecpp-def.h"
#include <math.h>
#include <time.h>

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

  double CoreMath::random() {
    return (double)std::rand() / (double)RAND_MAX;
  }

  CoreMath &Math = CoreMath::instance();
}
