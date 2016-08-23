#pragma once
#include "nodecpp-def.h"
#include "events.h"
#include "buffer.h"

namespace nodecpp {
  class ServerResponse : public EventEmitter {
  public:
    ServerResponse();
    ~ServerResponse();

  public:
    Buffer data;

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}
