#pragma once
#include "util.h"
#include "async-wrap.h"

namespace nodecpp {
  template <typename T>
  class ReqWrap : public AsyncWrap {
  public:
    inline ReqWrap(AsyncWrap::ProviderType provider)
      : AsyncWrap(provider) {}
    inline ~ReqWrap() override { 
      CHECK_EQ(req_.data, this);
    }
    inline void Dispatched() { req_.data = this; } // Call this after the req has been dispatched.

  public:
    T req_;  // Must be last.  TODO(bnoordhuis) Make private.
  };
}