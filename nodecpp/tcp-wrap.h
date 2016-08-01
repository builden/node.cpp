#pragma once
#include "connection-wrap.h"
#include "net.h"

namespace nodecpp {
  class TCPWrap : public ConnectionWrap<TCPWrap, uv_tcp_t> {
  public:
    TCPWrap(AsyncWrap* parent);

    int SetNoDelay(bool enable);
    int SetKeepAlive(int enable, int delay);
    int Bind(const string& ipAddress, int port);
    int Bind6(const string& ip6Adreess, int port);
    int Listen(int backlog);
    int Connect(const string& ipAddress, int port, ConnectCb_t cb = nullptr);
    int Connect6(const string& ip6Address, int port, ConnectCb_t cb = nullptr);

  private:
    static void AfterConnect(uv_connect_t* req, int status);
  };
}