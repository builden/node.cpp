#pragma once
#include "nodecpp-def.h"
#include "events.h"
#include "buffer.h"

struct uv_stream_s;
namespace nodecpp {
  class Socket : public EventEmitter {
  public:
    Socket(bool isTcp = true);
    ~Socket();

    void connect(const string& pipePath);
    void connect(uint32_t port, const string& host);
    void accept(uv_stream_s* server);
    void read();
    void write(const Buffer& buf);
    void end();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  using SocketPtr_t = shared_ptr<Socket>;
}