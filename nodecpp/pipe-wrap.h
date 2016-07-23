#pragma once
#include "stream-wrap.h"
#include "net.h"

namespace nodecpp {
  class PipeWrap : public StreamWrap {
  public:
    PipeWrap(bool ipc, AsyncWrap* parent);
    int bind(const string& name);
    int listen(int backlog = 0);
    void connect(const string& name, ConnectCb_t cb = nullptr);
    int open(int fd);

  private:
    static void OnConnection(uv_stream_t* handle, int status);
    static void AfterConnect(uv_connect_t* req, int status);

  private:
    uv_pipe_t handle_;
  };
}