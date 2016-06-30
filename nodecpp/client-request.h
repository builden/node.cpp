#pragma once
#include "nodecpp-def.h"
#include "events.h"

namespace nodecpp {
  class ClientRequest : public EventEmitter {
  public:
    ClientRequest();
    ~ClientRequest();

    void setHeader(const string& name, const string& value);
    string getHeader(const string& name);
    void removeHeader(const string& name);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}