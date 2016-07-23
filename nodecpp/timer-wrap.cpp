#include "timer-wrap.h"
#include "util.h"

namespace nodecpp {

  TimerWrap::TimerWrap() 
    : HandleWrap((uv_handle_t *)&handle_, AsyncWrap::PROVIDER_TIMERWRAP) {
    int r = uv_timer_init(uv_default_loop(), &handle_);
    CHECK_EQ(r, 0);
  }

  int TimerWrap::start(int64_t timeout, int64_t repeat, TimerCb_t cb) {
    cb_ = cb;
    return uv_timer_start(&handle_, onTimeout, timeout, repeat);
  }

  int TimerWrap::stop() {
    return uv_timer_stop(&handle_);
  }

  void TimerWrap::onTimeout(uv_timer_t* handle) {
    TimerWrap* wrap = static_cast<TimerWrap*>(handle->data);
    if (wrap->cb_) wrap->cb_();
  }
}