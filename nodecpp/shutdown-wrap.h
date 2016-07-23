#pragma once
#include "req-wrap.h"
#include "stream-base.h"

namespace nodecpp {
  class StreamBase;
  class ShutdownWrap : public ReqWrap<uv_shutdown_t>,
    public StreamReq<ShutdownWrap> {

  public:
    ShutdownWrap(StreamBase* wrap, DoneCb cb)
      : ReqWrap(AsyncWrap::PROVIDER_SHUTDOWNWRAP),
        StreamReq<ShutdownWrap>(cb),
        wrap_(wrap) {}

    inline StreamBase* wrap() const { return wrap_; }

  private:
    StreamBase* const wrap_;
  };
}