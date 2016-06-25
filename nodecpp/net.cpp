#include "net.h"
#include "error.h"
#include "callback.h"
#include "process.h"

namespace nodecpp {
  using ConnectionCb = Callback<ConnectionCb_t>;

  ServerPtr_t Net::createServer(ConnectionCb_t cb) {
    return shared_ptr<Server>(new Server(cb));
  }

  SocketPtr_t Net::connect(const string& pipePath, ConnectCb_t cb) {
    auto sock = shared_ptr<Socket>(new Socket(false));
    sock->connect(pipePath);
    return sock;
  }

  SocketPtr_t Net::connect(uint32_t port, const string& host, ConnectCb_t cb) {
    auto sock = shared_ptr<Socket>(new Socket);
    sock->connect(port, host);
    return sock;
  }

  Net &net = Net::instance();
}
