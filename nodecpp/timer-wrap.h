#pragma once
#include "handle-wrap.h"
#include "timers.h"
#include <uv.h>

namespace nodecpp {
  class TimerWrap : public HandleWrap {
  public:
    TimerWrap();
    int start(int64_t timeout, int64_t repeat, TimerCb_t cb);
    int stop();

  private:
    static void onTimeout(uv_timer_t* handle);

  private:
    uv_timer_t handle_;
    TimerCb_t cb_;
  };
}