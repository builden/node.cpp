#include "http-server.h"

namespace nodecpp {
  class HttpServer::impl {
  public:
    impl() {}

  public:
    ConnectionCb_t tcpCb_;
  };

  HttpServer::HttpServer(RequestListener_t& /*cb*/) : pimpl(new impl), Server(pimpl->tcpCb_) {}
  HttpServer::~HttpServer() {}
}