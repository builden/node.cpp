#pragma once
#include "nodecpp-def.h"
#include "events.h"

namespace nodecpp {
  class ChildProcess : public EventEmitter {
  public:
    void spawn();
  };
}