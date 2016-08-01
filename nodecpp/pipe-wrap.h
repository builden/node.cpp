#pragma once
#include "connection-wrap.h"
#include "net.h"

namespace nodecpp {
  class PipeWrap : public ConnectionWrap<PipeWrap, uv_pipe_t> {
  public:
    PipeWrap(bool ipc = true, AsyncWrap* parent = nullptr);
    int bind(const string& name); 
    int listen(int backlog = 0);
    void connect(const string& name, ConnectCb_t cb = nullptr);
    int open(int fd);

  private:
    static void AfterConnect(uv_connect_t* req, int status);
  };
}