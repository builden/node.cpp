#ifndef __NET_H__
#define __NET_H__
#include "nodecpp-def.h"
#include "events.h"
#include "buffer.h"
#include "singleton.h"
#include <uv.h>

#include "json11/json11.hpp"
using namespace json11;

namespace nodecpp {

  using std::bind;

  class Socket;
  class Server;
  using SocketPtr_t = shared_ptr<Socket>;
  using ServerPtr_t = shared_ptr<Server>;

  class Socket : public EventEmitter {
  public:
    Socket(bool isTcp = true);

    static void newBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

    void connect(const string& pipePath);
    void connect(uint32_t port, const string& host);
    void accept(uv_stream_t* server);
    void read();
    void write(const Buffer& buf);

  private:
    static void connectCb(uv_connect_t* req, int status);
    static void readCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
    static void writeCb(uv_write_t *req, int status);

  private:
    uv_pipe_t pipeClient_;
    uv_tcp_t tcpClient_;
    bool isTcp_;
  };

  
  using ConnectionCb_t = function<void(SocketPtr_t socket)>;
  using ConnectCb_t = function<void(void)>;

  class Server: public EventEmitter {
  public:
    Server(ConnectionCb_t& cb);
    void listen(const string& pipePath);
    void listen(uint32_t port);

    string address();

  private:
    static void connectionCb(uv_stream_t *server, int status);

  private:
    string pipePath_;
    uv_pipe_t pipeServer_;
    uv_tcp_t tcpServer_;
    bool isTcp_ = true;
    ConnectionCb_t cb_;
  };

  class Net : public Singleton<Net> {
  public:
    ServerPtr_t createServer(ConnectionCb_t cb);
    SocketPtr_t connect(const string& pipePath, ConnectCb_t cb);
    SocketPtr_t connect(uint32_t port, const string& host, ConnectCb_t cb);
  };

  extern Net &net;
}

#endif // !__NET_H__
