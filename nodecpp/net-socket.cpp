#include "net-socket.h"
#include "error.h"
#include <uv.h>

namespace nodecpp {
  class Socket::impl {
  public:
    impl(bool isTcp, Socket* sock) : isTcp_(isTcp), sock_(sock) {
      if (isTcp_) {
        uv_tcp_init(uv_default_loop(), &tcpClient_);
        tcpClient_.data = sock;
      }
      else {
        uv_pipe_init(uv_default_loop(), &pipeClient_, 0);
        pipeClient_.data = sock;
      }
    }

    void connect(const string& pipePath);
    void connect(uint32_t port, const string& host);
    void accept(uv_stream_t* server);
    void read();
    void write(const Buffer& buf);

  private:
    static void newBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void connectCb(uv_connect_t* req, int status);
    static void readCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
    static void writeCb(uv_write_t *req, int status);

  private:
    uv_pipe_t pipeClient_;
    uv_tcp_t tcpClient_;
    bool isTcp_;
    Socket* sock_;
  };

  Socket::~Socket() {}
  
  Socket::Socket(bool isTcp /*= true*/) : pimpl(new impl(isTcp, this)) {}

  void Socket::connect(const string& pipePath) {
    pimpl->connect(pipePath);
  }

  void Socket::connect(uint32_t port, const string& host) {
    pimpl->connect(port, host);
  }

  void Socket::accept(uv_stream_t* server) {
    pimpl->accept(server);
  }

  void Socket::read() {
    pimpl->read();
  }

  void Socket::write(const Buffer& buf) {
    pimpl->write(buf);
  }

  void Socket::impl::newBuffer(uv_handle_t* /*handle*/, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
  }

  void Socket::impl::connect(const string& pipePath) {
    auto req = new uv_connect_t;
    uv_pipe_connect(req, &pipeClient_, pipePath.c_str(), connectCb);
  }

  void Socket::impl::connect(uint32_t port, const string& host) {
    auto req = new uv_connect_t;
    struct sockaddr_in dest;
    uv_ip4_addr(host.c_str(), port, &dest);
    uv_tcp_connect(req, &tcpClient_, (const struct sockaddr*)&dest, connectCb);
  }

  void Socket::impl::connectCb(uv_connect_t* req, int status) {
    auto sock = (Socket *)req->handle->data;
    if (status < 0) {
      sock->emit("error", Error(status));
      return;
    }
    sock->emit("connect");
    sock->read();
    delete req;
  }

  void Socket::impl::readCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
    auto sock = (Socket*)client->data;
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

  void Socket::impl::read() {
    auto stream = isTcp_ ? (uv_stream_t*)&tcpClient_ : (uv_stream_t*)&pipeClient_;
    uv_read_start(stream, newBuffer, readCb);
  }

  void Socket::impl::write(const Buffer& buf) {
    auto wreq = new uv_write_t;
    uv_buf_t wrbuf = uv_buf_init(const_cast<char*>(buf.data()), buf.length());
    auto stream = isTcp_ ? (uv_stream_t*)&tcpClient_ : (uv_stream_t*)&pipeClient_;
    uv_write(wreq, stream, &wrbuf, 1, writeCb);
  }

  void Socket::impl::writeCb(uv_write_t *req, int status) {
    if (status < 0) {

    }
    delete req;
  }

  void Socket::impl::accept(uv_stream_t* server) {
    auto stream = isTcp_ ? (uv_stream_t*)&tcpClient_ : (uv_stream_t*)&pipeClient_;
    if (uv_accept(server, stream) == 0) {
      uv_read_start(stream, newBuffer, readCb);
    }
    else {
      uv_close((uv_handle_t*)&stream, NULL);
    }
  }
}