#include "super-agent.h"

namespace nodecpp {
  RequestWrap& SuperAgent::request(const string& method, const string& fullUrl) {
    auto wrap = new RequestWrap();
    wrap->request(method, fullUrl);
    return *wrap;
  }

  RequestWrap& SuperAgent::get(const string& fullUrl) {
    return request("GET", fullUrl);
  }

  RequestWrap& SuperAgent::post(const string& fullUrl) {
    return request("POST", fullUrl);
  }

  SuperAgent& sa = SuperAgent::instance();
}