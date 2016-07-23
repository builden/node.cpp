#include "stream-wrap.h"
#include "nodecpp-internals.h"
#include "shutdown-wrap.h"
#include "write-wrap.h"

namespace nodecpp {
  StreamWrap::StreamWrap(uv_stream_t* stream,
                         AsyncWrap::ProviderType provider,
                         AsyncWrap* parent /*= nullptr*/)
    : stream_(stream),
      HandleWrap((uv_handle_t*)stream, provider, parent) {

  }

  StreamWrap::~StreamWrap() {}

  int StreamWrap::DoShutdown(ShutdownWrap* req_wrap) {
    int err = uv_shutdown(&req_wrap->req_, stream(), AfterShutdown);
    req_wrap->Dispatched();
    return err;
  }

  int StreamWrap::DoTryWrite(uv_buf_t** bufs, size_t* count) {
    int err;
    size_t written;
    uv_buf_t* vbufs = *bufs;
    size_t vcount = *count;

    err = uv_try_write(stream(), vbufs, vcount);
    if (err == UV_ENOSYS || err == UV_EAGAIN)
      return 0;
    if (err < 0)
      return err;

    // Slice off the buffers: skip all written buffers and slice the one that
    // was partially written.
    written = err;
    for (; vcount > 0; vbufs++, vcount--) {
      // Slice
      if (vbufs[0].len > written) {
        vbufs[0].base += written;
        vbufs[0].len -= written;
        written = 0;
        break;

        // Discard
      }
      else {
        written -= vbufs[0].len;
      }
    }

    *bufs = vbufs;
    *count = vcount;

    return 0;
  }

  int StreamWrap::DoWrite(WriteWrap* w, uv_buf_t* bufs, size_t count, uv_stream_t* send_handle) {
    int r;
    if (send_handle == nullptr) {
      r = uv_write(&w->req_, stream(), bufs, count, AfterWrite);
    }
    else {
      r = uv_write2(&w->req_, stream(), bufs, count, send_handle, AfterWrite);
    }

    if (!r) {
      size_t bytes = 0;
      for (size_t i = 0; i < count; i++)
        bytes += bufs[i].len;
      if (stream()->type == UV_TCP) {
        // NODE_COUNT_NET_BYTES_SENT(bytes);
      }
      else if (stream()->type == UV_NAMED_PIPE) {
        // NODE_COUNT_PIPE_BYTES_SENT(bytes);
      }
    }

    w->Dispatched();
    // UpdateWriteQueueSize();

    return r;
  }

  void StreamWrap::OnAlloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
    CHECK_EQ(wrap->stream(), reinterpret_cast<uv_stream_t*>(handle));

    return static_cast<StreamBase*>(wrap)->OnAlloc(suggested_size, buf);
  }

  void StreamWrap::OnRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
    uv_handle_type type = UV_UNKNOWN_HANDLE;

    if (wrap->is_named_pipe_ipc() &&
      uv_pipe_pending_count(reinterpret_cast<uv_pipe_t*>(handle)) > 0) {
      type = uv_pipe_pending_type(reinterpret_cast<uv_pipe_t*>(handle));
    }

    OnReadCommon(handle, nread, buf, type);
  }

  void StreamWrap::OnReadCommon(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf, uv_handle_type pending) {
    StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);

    // We should not be getting this callback if someone as already called
    // uv_close() on the handle.
    // CHECK_EQ(wrap->persistent().IsEmpty(), false);

    if (nread > 0) {
      if (wrap->is_tcp()) {
        // NODE_COUNT_NET_BYTES_RECV(nread);
      }
      else if (wrap->is_named_pipe()) {
        // NODE_COUNT_PIPE_BYTES_RECV(nread);
      }
    }

    static_cast<StreamBase*>(wrap)->OnRead(nread, buf, pending);
  }

  void StreamWrap::AfterWrite(uv_write_t* req, int status) {
    WriteWrap* req_wrap = ContainerOf(&WriteWrap::req_, req);
    req_wrap->Done(status);
  }

  void StreamWrap::AfterShutdown(uv_shutdown_t* req, int status) {
    ShutdownWrap* req_wrap = ContainerOf(&ShutdownWrap::req_, req);
    req_wrap->Done(status);
  }

  void StreamWrap::OnAfterWriteImpl(WriteWrap* /*w*/, void* /*ctx*/) {
    // StreamWrap* wrap = static_cast<StreamWrap*>(ctx);
    // wrap->UpdateWriteQueueSize();
  }

  void StreamWrap::OnAllocImpl(size_t size, uv_buf_t* buf, void* /*ctx*/) {
    buf->base = static_cast<char*>(malloc(size));
    buf->len = size;

    if (buf->base == nullptr && size > 0) {
      FatalError(
        "node::StreamWrap::DoAlloc(size_t, uv_buf_t*, void*)",
        "Out Of Memory");
    }
  }

  void StreamWrap::OnReadImpl(ssize_t nread, const uv_buf_t* buf, uv_handle_type pending, void* /*ctx*/) {
    // StreamWrap* wrap = static_cast<StreamWrap*>(ctx);

    if (nread < 0) {
      if (buf->base != nullptr)
        free(buf->base);
      // wrap->EmitData(nread, Local<Object>(), pending_obj);
      return;
    }

    if (nread == 0) {
      if (buf->base != nullptr)
        free(buf->base);
      return;
    }

    // char* base = static_cast<char*>(realloc(buf->base, nread));
    CHECK_LE(static_cast<size_t>(nread), buf->len);

    if (pending == UV_TCP) {
      // pending_obj = AcceptHandle<TCPWrap, uv_tcp_t>(env, wrap);
    }
    else if (pending == UV_NAMED_PIPE) {
      // pending_obj = AcceptHandle<PipeWrap, uv_pipe_t>(env, wrap);
    }
    else if (pending == UV_UDP) {
      // pending_obj = AcceptHandle<UDPWrap, uv_udp_t>(env, wrap);
    }
    else {
      CHECK_EQ(pending, UV_UNKNOWN_HANDLE);
    }

    // wrap->EmitData(nread, obj, pending_obj);
  }
}