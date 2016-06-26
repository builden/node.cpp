#include "http.h"
#include "timers.h"

namespace nodecpp {

  HttpServerPtr_t Http::createServer(RequestListener_t cb) {
    return shared_ptr<HttpServer>(new HttpServer(cb));
  }

  ClientRequest& Http::get(const string& /*url*/, requestCb_t cb) {
    Json opts = Json::object{};
    return request(opts, cb);
  }

  ClientRequest& Http::request(Json& /*options*/, requestCb_t /*cb*/) {
    auto req = new ClientRequest();
    return *req;
  }

  Http& http = Http::instance();
}