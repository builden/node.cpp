#pragma once
#include "nodecpp-def.h"
#include "net-server.h"
#include "client-request.h"
#include "server-response.h"

namespace nodecpp {
  using RequestListener_t = function<void(const ClientRequest&, const ServerResponse&)>;

  class HttpServer : public Server {
  public:
    HttpServer(RequestListener_t& /*cb*/);
    ~HttpServer();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  using HttpServerPtr_t = shared_ptr<HttpServer>;
}