#pragma once
#include "net-socket.h"

namespace nodecpp {
  using ConnectionCb_t = function<void(SocketPtr_t socket)>;

  class Server : public EventEmitter {
  public:
    Server(ConnectionCb_t& cb);
    ~Server();
    void listen(const string& pipePath);
    void listen(uint32_t port);

    string address();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  using ServerPtr_t = shared_ptr<Server>;
}