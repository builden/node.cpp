#pragma once
#include "nodecpp-def.h"
#include "events.h"

namespace nodecpp {
  class ServerResponse : public EventEmitter {
  public:
    ServerResponse();
    ~ServerResponse();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}
