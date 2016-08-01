#pragma once
#include "req-wrap.h"
#include "stream-base.h"
#include "nodecpp-internals.h"

namespace nodecpp {
  class WriteWrap : public ReqWrap<uv_write_t>,
    public StreamReq<WriteWrap> {
  public:
    static inline WriteWrap* New(
      StreamBase* wrap,
      DoneCb cb,
      size_t extra = 0) {
      size_t storage_size = ROUND_UP(sizeof(WriteWrap), kAlignSize) + extra;
      char* storage = new char[storage_size];

      return new(storage) WriteWrap(wrap, cb, storage_size);
    }

    WriteWrap(StreamBase* wrap, DoneCb cb, size_t storage_size)
      : ReqWrap(AsyncWrap::PROVIDER_WRITEWRAP),
        StreamReq<WriteWrap>(cb),
        wrap_(wrap),
        storage_size_(storage_size) {}

    void Dispose();
    inline StreamBase* wrap() const { return wrap_; }

    static const size_t kAlignSize = 16;

    void* operator new(size_t /*size*/) = delete;
    void* operator new(size_t /*size*/, char* storage) { return storage; }

    // This is just to keep the compiler happy. It should never be called, since
    // we don't use exceptions in node.
    void operator delete(void* /*ptr*/, char* /*storage*/) { UNREACHABLE(); }

  private:
    // People should not be using the non-placement new and delete operator on a
    // WriteWrap. Ensure this never happens.
    void operator delete(void* /*ptr*/) { UNREACHABLE(); }

  private:
    StreamBase* wrap_;
    const size_t storage_size_;
  };
}