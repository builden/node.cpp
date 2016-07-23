#pragma once
#include "util.h"
#include "callback.h"
#include "async-wrap.h"

namespace nodecpp {
  template <typename T>
  class ReqWrap : public AsyncWrap {
  public:
    inline ReqWrap(CallbackBase* cbWrap, AsyncWrap::ProviderType provider)
      : cbWrap_(cbWrap), AsyncWrap(provider) {}
    inline ~ReqWrap() override { 
      CHECK_EQ(req_.data, this);
      if (cbWrap_) delete cbWrap_;
    }
    inline void Dispatched() { req_.data = this; } // Call this after the req has been dispatched.

  public:
    T req_;  // Must be last.  TODO(bnoordhuis) Make private.
    CallbackBase* cbWrap_;
  };
}