#pragma once
#include "stream-wrap.h"
#include "net.h"

namespace nodecpp {
  class TCPWrap : StreamWrap {
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
    static void OnConnection(uv_stream_t* handle, int status);
    static void AfterConnect(uv_connect_t* req, int status);

  private:
    uv_tcp_t handle_;
  };
}