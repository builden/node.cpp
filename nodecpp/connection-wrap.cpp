#include "connection-wrap.h"
#include "pipe-wrap.h"
#include "tcp-wrap.h"

namespace nodecpp {

  template <typename WrapType, typename UVType>
  ConnectionWrap<WrapType, UVType>::ConnectionWrap(
    ProviderType provider,
    AsyncWrap* parent)
    : StreamWrap(
      reinterpret_cast<uv_stream_t*>(&handle_),
      provider,
      parent) {}


  template <typename WrapType, typename UVType>
  void ConnectionWrap<WrapType, UVType>::OnConnection(uv_stream_t* handle,
    int status) {
    WrapType* wrap_data = static_cast<WrapType*>(handle->data);
    CHECK_NE(wrap_data, nullptr);
    CHECK_EQ(&wrap_data->handle_, reinterpret_cast<UVType*>(handle));

    if (status == 0) {
/*
      uv_stream_t* client_handle =
        reinterpret_cast<uv_stream_t*>(&wrap->handle_);
      // uv_accept can fail if the new connection has already been closed, in
      // which case an EAGAIN (resource temporarily unavailable) will be
      // returned.
      if (uv_accept(handle, client_handle))
        return;*/

      // Successful accept. Call the onconnection callback in JavaScript land.
    }
    // wrap_data->MakeCallback(env->onconnection_string(), arraysize(argv), argv);
  }

  template ConnectionWrap<PipeWrap, uv_pipe_t>::ConnectionWrap(
    ProviderType provider,
    AsyncWrap* parent);

  template ConnectionWrap<TCPWrap, uv_tcp_t>::ConnectionWrap(
    ProviderType provider,
    AsyncWrap* parent);

  template void ConnectionWrap<PipeWrap, uv_pipe_t>::OnConnection(
    uv_stream_t* handle, int status);

  template void ConnectionWrap<TCPWrap, uv_tcp_t>::OnConnection(
    uv_stream_t* handle, int status);
}