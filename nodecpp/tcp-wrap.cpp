#include "tcp-wrap.h"
#include "req-wrap.h"

namespace nodecpp {
  class TCPConnectWrap : public ReqWrap<uv_connect_t> {
  public:
    TCPConnectWrap(ConnectCb_t cb)
      : cb_(cb), ReqWrap(AsyncWrap::PROVIDER_PIPECONNECTWRAP) {}

    ConnectCb_t cb_;
  };

  TCPWrap::TCPWrap(AsyncWrap* parent) 
    : StreamWrap(reinterpret_cast<uv_stream_t*>(&handle_),
      AsyncWrap::PROVIDER_TCPWRAP,
      parent) {
    int r = uv_tcp_init(uv_default_loop(), &handle_);
    CHECK_EQ(r, 0);  // How do we proxy this error up to javascript?
                     // Suggestion: uv_tcp_init() returns void.
  }

  int TCPWrap::SetNoDelay(bool enable) {
    return uv_tcp_nodelay(&handle_, enable);
  }

  int TCPWrap::SetKeepAlive(int enable, int delay) {
    return uv_tcp_keepalive(&handle_, enable, delay);
  }

  int TCPWrap::Bind(const string& ipAddress, int port) {
    sockaddr_in addr;
    int err = uv_ip4_addr(ipAddress.c_str(), port, &addr);
    if (err == 0) {
      err = uv_tcp_bind(&handle_,
        reinterpret_cast<const sockaddr*>(&addr),
        0);
    }
    return err;
  }

  int TCPWrap::Bind6(const string& ip6Adreess, int port) {
    sockaddr_in6 addr;
    int err = uv_ip6_addr(ip6Adreess.c_str(), port, &addr);
    if (err == 0) {
      err = uv_tcp_bind(&handle_,
        reinterpret_cast<const sockaddr*>(&addr),
        0);
    }
    return err;
  }

  int TCPWrap::Listen(int backlog) {
    return uv_listen(reinterpret_cast<uv_stream_t*>(&handle_),
      backlog,
      OnConnection);
  }

  int TCPWrap::Connect(const string& ipAddress, int port, ConnectCb_t cb /*= nullptr*/) {
    sockaddr_in addr;
    int err = uv_ip4_addr(ipAddress.c_str(), port, &addr);

    if (err == 0) {
      TCPConnectWrap* req_wrap = new TCPConnectWrap(cb);
      err = uv_tcp_connect(&req_wrap->req_,
        &handle_,
        reinterpret_cast<const sockaddr*>(&addr),
        AfterConnect);
      req_wrap->Dispatched();
      if (err)
        delete req_wrap;
    }
    return err;
  }

  int TCPWrap::Connect6(const string& ip6Address, int port, ConnectCb_t cb /*= nullptr*/) {
    sockaddr_in6 addr;
    int err = uv_ip6_addr(ip6Address.c_str(), port, &addr);

    if (err == 0) {
      TCPConnectWrap* req_wrap = new TCPConnectWrap(cb);
      err = uv_tcp_connect(&req_wrap->req_,
        &handle_,
        reinterpret_cast<const sockaddr*>(&addr),
        AfterConnect);
      req_wrap->Dispatched();
      if (err)
        delete req_wrap;
    }
    return err;
  }

  void TCPWrap::OnConnection(uv_stream_t* handle, int /*status*/) {
    TCPWrap* tcp_wrap = static_cast<TCPWrap*>(handle->data);
    CHECK_EQ(&tcp_wrap->handle_, reinterpret_cast<uv_tcp_t*>(handle));

/*
    Local<Value> argv[2] = {
      Integer::New(env->isolate(), status),
      Undefined(env->isolate())
    };

    if (status == 0) {
      // Instantiate the client javascript object and handle.
      Local<Object> client_obj =
        Instantiate(env, static_cast<AsyncWrap*>(tcp_wrap));

      // Unwrap the client javascript object.
      TCPWrap* wrap = Unwrap<TCPWrap>(client_obj);
      CHECK_NE(wrap, nullptr);
      uv_stream_t* client_handle = reinterpret_cast<uv_stream_t*>(&wrap->handle_);
      if (uv_accept(handle, client_handle))
        return;

      // Successful accept. Call the onconnection callback in JavaScript land.
      argv[1] = client_obj;
    }

    tcp_wrap->MakeCallback(env->onconnection_string(), arraysize(argv), argv);*/
  }

  void TCPWrap::AfterConnect(uv_connect_t* req, int /*status*/) {
    TCPConnectWrap* req_wrap = static_cast<TCPConnectWrap*>(req->data);
    // TCPWrap* wrap = static_cast<TCPWrap*>(req->handle->data);

    req_wrap->cb_();
    delete req_wrap;
  }

}