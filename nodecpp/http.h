#ifndef __HTTP_H__
#define __HTTP_H__

#include "nodecpp-def.h"
#include "singleton.h"
#include "events.h"
#include "net.h"
#include "http-server.h"
#include "http-common.h"

namespace nodecpp {
  using requestCb_t = function<void(const ServerResponse&)>;

  class Http : public Singleton<Http>,
               public EventEmitter {
  public:
    HttpServerPtr_t createServer(RequestListener_t cb);
    ClientRequest& get(const string& url, requestCb_t cb);
    ClientRequest& request(Json& options, requestCb_t cb);

    const svec_t& METHODS = httpMethods;
    const ismap_t& STATUS_CODES = statusCodes;
  };

  extern Http& http;
}

#endif // !__HTTP_H__
