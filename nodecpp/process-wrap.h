#pragma once
#include "nodecpp-def.h"

namespace nodecpp {
  class ProcessWrap {
  public:
    ProcessWrap();
    ~ProcessWrap();

    void spawn();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}