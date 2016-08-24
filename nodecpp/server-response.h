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
    uint32_t statusCode;
    string statusMessage;
    ssmap_t headers;  // key��Сд

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}
