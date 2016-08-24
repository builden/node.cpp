#pragma once
#include "nodecpp-def.h"
#include "error.h"
#include "server-response.h"

namespace nodecpp {
  using RequestWrapCb_t = function<void(const Error&, const ServerResponse&)>;
  // 第一个参数已经下载了多少byte
  // 第二个参数总共多少byte，
  using ProgressCb_t = function<void(uint32_t, uint32_t)>;
  class RequestWrap {
  public:
    RequestWrap();
    void request(const string& method, const string& fullUrl);
    RequestWrap& set(const string& headerKey, const string& headerValue);
    // 发送到后台的格式，默认是application/json
    // 可设置成application/x-www-form-urlencoded
    RequestWrap& accept(const string& type);
    RequestWrap& send(json ctx);
    RequestWrap& end(RequestWrapCb_t cb);
    RequestWrap& onProgress(ProgressCb_t cb);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}