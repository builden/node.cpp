#pragma once
#include "util.h"
#include "req-wrap.h"

namespace nodecpp {
  template <typename T>
  ReqWrap<T>::ReqWrap(CallbackBase* cb, AsyncWrap::ProviderType provider)
    : cb_(cb), AsyncWrap(provider) {

    // FIXME(bnoordhuis) The fact that a reinterpret_cast is needed is
    // arguably a good indicator that there should be more than one queue.
    // env->req_wrap_queue()->PushBack(reinterpret_cast<ReqWrap<uv_req_t>*>(this));
  }

  template <typename T>
  ReqWrap<T>::~ReqWrap() {
    CHECK_EQ(req_.data, this);  // Assert that someone has called Dispatched().
  }

  template <typename T>
  void ReqWrap<T>::Dispatched() {
    req_.data = this;
  }
}