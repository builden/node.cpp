#ifndef __HTTP_H__
#define __HTTP_H__

#include "nodecpp-def.h"
#include "singleton.h"
#include "events.h"
#include "net.h"

namespace nodecpp {
  class HttpServer;
  using HttpServerPtr_t = shared_ptr<HttpServer>;

  class ClientRequest : public EventEmitter {
  public:
  };

  class ServerResponse : public EventEmitter {
  public:
  };

  using requestListener_t = function<void(const ClientRequest&, const ServerResponse&)>;
  using requestCb_t = function<void(const ServerResponse&)>;

  class HttpServer : public Server {
  public:
    HttpServer(requestListener_t& /*cb*/) : Server(tcpCb_) {
    }

    ConnectionCb_t tcpCb_;
  };

  class Http : public Singleton<Http>,
               public EventEmitter {
  public:
    HttpServerPtr_t createServer(requestListener_t cb);
    ClientRequest& get(const string& url, requestCb_t cb);
    ClientRequest& request(Json& options, requestCb_t cb);
  };

  extern Http& http;
}

#endif // !__HTTP_H__
