#pragma once
#include "async-wrap.h"

namespace nodecpp {
  class CallbackBase {
  public:
  };

  template <typename callback_t>
  class CallbackWrap: public CallbackBase {
  public:
    CallbackWrap(const callback_t& cb) : cb_(cb) {}
    virtual ~CallbackWrap() {}
  private:
    callback_t cb_;
  };

  template <typename T>
  class ReqWrap : public AsyncWrap {
  public:
    inline ReqWrap(CallbackBase* cb, AsyncWrap::ProviderType provider);
    inline ~ReqWrap() override;
    inline void Dispatched();  // Call this after the req has been dispatched.

    template<typename ... Args>
      void invoke(Args& ... args) {
      cb_(std::forward<Args>(args)...);
      delete cb_;
    }

  private:
    // ListNode<ReqWrap> req_wrap_queue_;

  public:
    T req_;  // Must be last.  TODO(bnoordhuis) Make private.
    CallbackBase* cb_;
  };
}