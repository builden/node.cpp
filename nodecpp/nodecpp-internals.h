#pragma once
#include "util.h"

namespace nodecpp {
#ifndef ROUND_UP
# define ROUND_UP(a, b) ((a) % (b) ? ((a) + (b)) - ((a) % (b)) : (a))
#endif

  void FatalError(const char* location, const char* message);
  const char *signo_string(int errorno);
}