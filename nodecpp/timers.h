#ifndef __TIMERS_H__
#define __TIMERS_H__
#include "nodecpp-def.h"

namespace nodecpp {
  using TimerCb_t = function<void()>;

  int setTimeout(const TimerCb_t& cb, int after);
  void clearTimeout(int timer);

  int setInterval(const TimerCb_t& cb, int repeat);
  void clearInterval(int timer);
}

#endif // !__TIMERS_H__
