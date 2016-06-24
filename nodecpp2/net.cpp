#include "net.h"
#include "error.h"
#include "callback.h"
#include "process.h"

namespace nodepp {
  using ConnectionCb = Callback<ConnectionCb_t>;
  Socket::Socket(bool isTcp /*= true*/) : isTcp_(isTcp) {
    if (isTcp_) {
      uv_tcp_init(uv_default_loop(), &tcpClient_);
      tcpClient_.data = this;
    }
    else {
      uv_pipe_init(uv_default_loop(), &pipeClient_, 0);
      pipeClient_.data = this;
    }
  }

  void Socket::newBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
  }

  void Socket::connect(const string& pipePath) {
    auto req = new uv_connect_t;
    uv_pipe_connect(req, &pipeClient_, pipePath.c_str(), connectCb);
  }

  void Socket::connect(uint32_t port, const string& host) {
    auto req = new uv_connect_t;
    struct sockaddr_in dest;
    uv_ip4_addr(host.c_str(), port, &dest);
    uv_tcp_connect(req, &tcpClient_, (const struct sockaddr*)&dest, connectCb);
  }

  void Socket::connectCb(uv_connect_t* req, int status) {
    auto sock = (Socket *)req->handle->data;
    if (status < 0) {
      sock->emit("error", Error(status));
      return;
    }
    sock->emit("connect");
    sock->read();
    delete req;
  }

  void Socket::readCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
    auto sock = (Server*)client->data;
    if (nread < 0) {
      if (nread == UV_EOF) {
        sock->emit("end");
      }
      else {
        sock->emit("error", Error(nread));
      }
      uv_close((uv_handle_t*)client, NULL);
      return;
    }

    sock->emit<const Buffer&>("data", Buffer(buf->base, nread));
    delete[] buf->base;
  }

  void Socket::read() {
    auto stream = isTcp_ ? (uv_stream_t*)&tcpClient_ : (uv_stream_t*)&pipeClient_;
    uv_read_start(stream, newBuffer, readCb);
  }

  void Socket::write(const Buffer& buf) {
    auto wreq = new uv_write_t;
    uv_buf_t wrbuf = uv_buf_init(const_cast<char*>(buf.data()), buf.length());
    auto stream = isTcp_ ? (uv_stream_t*)&tcpClient_ : (uv_stream_t*)&pipeClient_;
    uv_write(wreq, stream, &wrbuf, 1, writeCb);
  }

  void Socket::writeCb(uv_write_t *req, int status) {
    if (status < 0) {
      
    }
    delete req;
  }

  void Socket::accept(uv_stream_t* server) {
    auto stream = isTcp_ ? (uv_stream_t*)&tcpClient_ : (uv_stream_t*)&pipeClient_;
    if (uv_accept(server, stream) == 0) {
      uv_read_start(stream, newBuffer, readCb);
    }
    else {
      uv_close((uv_handle_t*)&stream, NULL);
    }
  }

  Server::Server(ConnectionCb_t& cb) : cb_(cb) {
  }

  void Server::listen(const string& pipePath) {
    isTcp_ = false;
    uv_pipe_init(uv_default_loop(), &pipeServer_, 0);

    pipePath_ = pipePath;
    int r = 0;
    if ((r = uv_pipe_bind(&pipeServer_, pipePath_.c_str()))) {
      process.nextTick([this, r] {
        this->emit<const Error&>("error", Error(r));
      });
      return;
    }

    pipeServer_.data = this;
    if ((r = uv_listen((uv_stream_t*)&pipeServer_, 128, connectionCb))) {
      process.nextTick([this, r] {
        this->emit<const Error&>("error", Error(r));
      });
      return;
    }

    process.nextTick([this] {
      this->emit("listening");
    });
  }

  void Server::listen(uint32_t port) {
    uv_tcp_init(uv_default_loop(), &tcpServer_);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);

    int r = 0;
    if ((r = uv_tcp_bind(&tcpServer_, (const struct sockaddr*)&addr, 0))) {
      process.nextTick([this, r] {
        this->emit<const Error&>("error", Error(r));
      });
      return;
    }

    tcpServer_.data = this;
    if ((r = uv_listen((uv_stream_t*)&tcpServer_, 128, connectionCb))) {
      process.nextTick([this, r] {
        this->emit<const Error&>("error", Error(r));
      });
      return;
    }

    process.nextTick([&] {
      this->emit("listening");
    });
  }

  string Server::address() {
    return pipePath_;
  }

  void Server::connectionCb(uv_stream_t *server, int status) {
    auto svr = (Server *)server->data;
    if (status == -1) {
      svr->emit("error", "ConnectionCb Error");
    }

    auto sock = SocketPtr_t(new Socket(svr->isTcp_));
    svr->cb_(sock);
    sock->accept(server);
  }

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
