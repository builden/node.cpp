#ifndef __NET_H__
#define __NET_H__
#include "net-server.h"
#include "net-socket.h"
#include "singleton.h"

namespace nodecpp {
  using ConnectCb_t = function<void(void)>;

  class Net : public Singleton<Net> {
  public:
    ServerPtr_t createServer(ConnectionCb_t cb);
    SocketPtr_t connect(const string& pipePath, ConnectCb_t cb);
    SocketPtr_t connect(uint32_t port, const string& host, ConnectCb_t cb);
  };

  extern Net &net;
}

#endif // !__NET_H__
