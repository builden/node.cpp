#pragma once
#include "nodecpp-def.h"
#include "events.h"

namespace nodecpp {
  class ClientRequest : public EventEmitter {
  public:
    ClientRequest();
    ~ClientRequest();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}