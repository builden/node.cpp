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

    SuperAgent& request(const string& method, const string& url);
    SuperAgent& get(const string& url);
    SuperAgent& post(const string& url);
    SuperAgent& set(const string& headerKey, const string& headerValue);
    // 发送到后台的格式，默认是application/json
    // 可设置成application/x-www-form-urlencoded
    SuperAgent& accept(const string& type);
    SuperAgent& send(json ctx);
    void end(SuperAgentCb_t cb);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
  
  extern SuperAgent& sa;
}