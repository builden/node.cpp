#pragma once
#include "singleton.h"
#include "nodecpp-def.h"
#include "error.h"
#include "server-response.h"

namespace nodecpp {
  using SuperAgentCb_t = function<void(const Error&, const ServerResponse&)>;
  class SuperAgent : public Singleton<SuperAgent> {
  public:
    SuperAgent();

    SuperAgent& get(const string& url);
    SuperAgent& set(const string& headerKey, const string& headerValue);
    void end(SuperAgentCb_t cb);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
  
  extern SuperAgent& sa;
}