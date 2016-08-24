#pragma once
#include "singleton.h"
#include "request-wrap.h"

namespace nodecpp {

  class SuperAgent : public Singleton<SuperAgent> {
  public:
    RequestWrap& request(const string& method, const string& url);
    RequestWrap& get(const string& url);
    RequestWrap& post(const string& url);
  };
  
  extern SuperAgent& sa;
}