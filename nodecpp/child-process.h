#pragma once
#include "nodecpp-def.h"
#include "events.h"

namespace nodecpp {
  class ProcessWrap;
  class ChildProcess : public EventEmitter {
  public:
    ChildProcess();
    int spawn(json options);

  private:
    ProcessWrap* handle_ = nullptr;
    string spawnfile_;
    svec_t spawnargs_;
  };
}