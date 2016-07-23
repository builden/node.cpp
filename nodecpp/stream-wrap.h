#pragma once
#include "handle-wrap.h"
#include "stream-base.h"
#include <uv.h>

namespace nodecpp {
  class ShutdownWrap;
  class WriteWrap;

  class StreamWrap : public HandleWrap, public StreamBase {
  public:
    StreamWrap(uv_stream_t* stream,
      AsyncWrap::ProviderType provider,
      AsyncWrap* parent = nullptr);
    virtual ~StreamWrap();

    // Resource implementation
    int DoShutdown(ShutdownWrap* req_wrap) override;
    int DoTryWrite(uv_buf_t** bufs, size_t* count) override;
    int DoWrite(WriteWrap* w,
      uv_buf_t* bufs,
      size_t count,
      uv_stream_t* send_handle) override;

    inline uv_stream_t* stream() const {
      return stream_;
    }

    inline bool is_named_pipe() const {
      return stream()->type == UV_NAMED_PIPE;
    }

    inline bool is_named_pipe_ipc() const {
      return is_named_pipe() &&
        reinterpret_cast<const uv_pipe_t*>(stream())->ipc != 0;
    }

    inline bool is_tcp() const {
      return stream()->type == UV_TCP;
    }

  private:
    // Callbacks for libuv
    static void OnAlloc(uv_handle_t* handle,
      size_t suggested_size,
      uv_buf_t* buf);

    static void OnRead(uv_stream_t* handle,
      ssize_t nread,
      const uv_buf_t* buf);
    static void OnReadCommon(uv_stream_t* handle,
      ssize_t nread,
      const uv_buf_t* buf,
      uv_handle_type pending);
    static void AfterWrite(uv_write_t* req, int status);
    static void AfterShutdown(uv_shutdown_t* req, int status);

    // Resource interface implementation
    static void OnAfterWriteImpl(WriteWrap* w, void* ctx);
    static void OnAllocImpl(size_t size, uv_buf_t* buf, void* ctx);
    static void OnReadImpl(ssize_t nread,
      const uv_buf_t* buf,
      uv_handle_type pending,
      void* ctx);

  private:
    uv_stream_t* const stream_;
  };
}