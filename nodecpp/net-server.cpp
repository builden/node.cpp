#include "net-server.h"
#include "process.h"
#include "error.h"
#include <uv.h>

namespace nodecpp {
  class Server::impl {
  public:
    impl(ConnectionCb_t& cb, Server* svr) : cb_(cb), svr_(svr) {}

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
    Server* svr_;
  };

  Server::Server(ConnectionCb_t& cb) : pimpl(new impl(cb, this)) {}
  Server::~Server() {}

  void Server::listen(const string& pipePath) {
    pimpl->listen(pipePath);
  }

  void Server::listen(uint32_t port) {
    pimpl->listen(port);
  }

  string Server::address() {
    return pimpl->address();
  }

  void Server::impl::listen(const string& pipePath) {
    isTcp_ = false;
    uv_pipe_init(uv_default_loop(), &pipeServer_, 0);

    pipePath_ = pipePath;
    int r = 0;
    if ((r = uv_pipe_bind(&pipeServer_, pipePath_.c_str())) != 0) {
      process.nextTick([this, r] {
        svr_->emit<const Error&>("error", Error(r));
      });
      return;
    }

    pipeServer_.data = this;
    if ((r = uv_listen((uv_stream_t*)&pipeServer_, 128, connectionCb)) != 0) {
      process.nextTick([this, r] {
        svr_->emit<const Error&>("error", Error(r));
      });
      return;
    }

    process.nextTick([this] {
      svr_->emit("listening");
    });
  }

  void Server::impl::listen(uint32_t port) {
    uv_tcp_init(uv_default_loop(), &tcpServer_);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);

    int r = 0;
    if ((r = uv_tcp_bind(&tcpServer_, (const struct sockaddr*)&addr, 0)) != 0) {
      process.nextTick([this, r] {
        svr_->emit<const Error&>("error", Error(r));
      });
      return;
    }

    tcpServer_.data = this;
    if ((r = uv_listen((uv_stream_t*)&tcpServer_, 128, connectionCb)) != 0) {
      process.nextTick([this, r] {
        svr_->emit<const Error&>("error", Error(r));
      });
      return;
    }

    process.nextTick([&] {
      svr_->emit("listening");
    });
  }

  string Server::impl::address() {
    return pipePath_;
  }

  void Server::impl::connectionCb(uv_stream_t *server, int status) {
    auto svr = (Server::impl *)server->data;
    if (status == -1) {
      svr->svr_->emit("error", Error(status));
    }

    auto sock = SocketPtr_t(new Socket(svr->isTcp_));
    svr->cb_(sock);
    sock->accept(server);
  }
}