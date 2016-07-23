#include "pipe-wrap.h"
#include "req-wrap.h"

namespace nodecpp {
  class PipeConnectWrap : public ReqWrap<uv_connect_t> {
  public:
    PipeConnectWrap(ConnectCb_t cb) 
      : cb_(cb), ReqWrap(AsyncWrap::PROVIDER_PIPECONNECTWRAP) {}

    ConnectCb_t cb_;
  };

  PipeWrap::PipeWrap(bool ipc, AsyncWrap* parent)
    : StreamWrap(
      reinterpret_cast<uv_stream_t*>(&handle_),
      AsyncWrap::PROVIDER_PIPEWRAP,
      parent) {
    int r = uv_pipe_init(uv_default_loop(), &handle_, ipc);
    CHECK_EQ(r, 0);  // How do we proxy this error up to javascript?
                     // Suggestion: uv_pipe_init() returns void.
  }

  int PipeWrap::bind(const string& name) {
    return uv_pipe_bind(&handle_, name.c_str());
  }

  int PipeWrap::listen(int backlog /*= 0*/) {
    return uv_listen(reinterpret_cast<uv_stream_t*>(&handle_),
      backlog,
      OnConnection);
  }

  void PipeWrap::connect(const string& name, ConnectCb_t cb /*= nullptr*/) {
    PipeConnectWrap* req_wrap = new PipeConnectWrap(cb);
    uv_pipe_connect(&req_wrap->req_,
      &handle_,
      name.c_str(),
      AfterConnect);
    req_wrap->Dispatched();
  }

  int PipeWrap::open(int fd) {
    return uv_pipe_open(&handle_, fd);
  }

  void PipeWrap::OnConnection(uv_stream_t* handle, int status) {
    PipeWrap* pipe_wrap = static_cast<PipeWrap*>(handle->data);
    CHECK_EQ(&pipe_wrap->handle_, reinterpret_cast<uv_pipe_t*>(handle));

    if (status != 0) {
      // connect failed
      // pipe_wrap->MakeCallback(env->onconnection_string(), arraysize(argv), argv);
      return;
    }

//     uv_stream_t* client_handle = reinterpret_cast<uv_stream_t*>(&wrap->handle_);
//     if (uv_accept(handle, client_handle))
//       return;

    // Successful accept. Call the onconnection callback in JavaScript land.
    // argv[1] = client_obj;
    // pipe_wrap->MakeCallback(env->onconnection_string(), arraysize(argv), argv);
  }

  void PipeWrap::AfterConnect(uv_connect_t* req, int status) {
    PipeConnectWrap* req_wrap = static_cast<PipeConnectWrap*>(req->data);
    // PipeWrap* wrap = static_cast<PipeWrap*>(req->handle->data);

    bool readable = false, writable = false;

    if (status) {
      readable = writable = 0;
    }
    else {
      readable = uv_is_readable(req->handle) != 0;
      writable = uv_is_writable(req->handle) != 0;
    }

    req_wrap->cb_();

    delete req_wrap;
  }

}