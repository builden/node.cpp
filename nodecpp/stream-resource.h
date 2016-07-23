#pragma once
#include <uv.h>
// #include "write-wrap.h"
// #include "shutdown-wrap.h"

namespace nodecpp {
  class StreamBase;
  class WriteWrap;
  class ShutdownWrap;

  class StreamResource {
  public:
    template <class T>
    struct Callback {
      Callback() : fn(nullptr), ctx(nullptr) {}
      Callback(T fn, void* ctx) : fn(fn), ctx(ctx) {}
      Callback(const Callback&) = default;

      inline bool is_empty() { return fn == nullptr; }
      inline void clear() {
        fn = nullptr;
        ctx = nullptr;
      }

      T fn;
      void* ctx;
    };

    typedef void(*AfterWriteCb)(WriteWrap* w, void* ctx);
    typedef void(*AllocCb)(size_t size, uv_buf_t* buf, void* ctx);
    typedef void(*ReadCb)(ssize_t nread,
      const uv_buf_t* buf,
      uv_handle_type pending,
      void* ctx);

    StreamResource() : bytes_read_(0) {
    }
    virtual ~StreamResource() = default;

    virtual int DoShutdown(ShutdownWrap* req_wrap) = 0;
    virtual int DoTryWrite(uv_buf_t** bufs, size_t* count);
    virtual int DoWrite(WriteWrap* w,
      uv_buf_t* bufs,
      size_t count,
      uv_stream_t* send_handle) = 0;
    virtual const char* Error() const;
    virtual void ClearError();

    // Events
    inline void OnAfterWrite(WriteWrap* w) {
      if (!after_write_cb_.is_empty())
        after_write_cb_.fn(w, after_write_cb_.ctx);
    }

    inline void OnAlloc(size_t size, uv_buf_t* buf) {
      if (!alloc_cb_.is_empty())
        alloc_cb_.fn(size, buf, alloc_cb_.ctx);
    }

    inline void OnRead(ssize_t nread,
      const uv_buf_t* buf,
      uv_handle_type pending = UV_UNKNOWN_HANDLE) {
      if (nread > 0)
        bytes_read_ += static_cast<uint64_t>(nread);
      if (!read_cb_.is_empty())
        read_cb_.fn(nread, buf, pending, read_cb_.ctx);
    }

    inline void set_after_write_cb(Callback<AfterWriteCb> c) {
      after_write_cb_ = c;
    }

    inline void set_alloc_cb(Callback<AllocCb> c) { alloc_cb_ = c; }
    inline void set_read_cb(Callback<ReadCb> c) { read_cb_ = c; }

    inline Callback<AfterWriteCb> after_write_cb() { return after_write_cb_; }
    inline Callback<AllocCb> alloc_cb() { return alloc_cb_; }
    inline Callback<ReadCb> read_cb() { return read_cb_; }

  private:
    Callback<AfterWriteCb> after_write_cb_;
    Callback<AllocCb> alloc_cb_;
    Callback<ReadCb> read_cb_;
    uint64_t bytes_read_;

    friend class StreamBase;
  };
}