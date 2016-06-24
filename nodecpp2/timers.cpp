#include "timers.h"
#include <uv.h>
#include "callback.h"

namespace nodepp {
  using timer_t_vec = std::vector<uv_timer_t*>;
  timer_t_vec vecTimer;

  using TimerCb = Callback<TimerCb_t>;

  int setTimeout(const TimerCb_t& cb, int after) {
    uv_timer_t* timer = new uv_timer_t();
    uv_timer_init(uv_default_loop(), timer);

    vecTimer.emplace_back(timer);
    timer->data = new TimerCb(cb);
    uv_timer_start(timer, [](uv_timer_t* handle) {
      auto cb = (TimerCb*)handle->data;
      cb->invoke();
      delete cb;
      delete handle;

      auto fTimer = std::find(vecTimer.begin(), vecTimer.end(), handle);
      if (fTimer != vecTimer.end()) vecTimer.erase(fTimer);
    }, after, 0);
    
    return (int)timer;
  }

  void clearTimeout(int timer) {
    auto handle = (uv_timer_t *)timer;
    auto fTimer = std::find(vecTimer.begin(), vecTimer.end(), handle);
    if (fTimer != vecTimer.end()) {
      uv_timer_stop(handle);
      delete handle->data;
      delete handle;
      vecTimer.erase(fTimer);
    }
  }

  int setInterval(const TimerCb_t& cb, int repeat) {
    uv_timer_t* timer = new uv_timer_t();
    uv_timer_init(uv_default_loop(), timer);

    vecTimer.emplace_back(timer);
    timer->data = new TimerCb(cb);
    uv_timer_start(timer, [](uv_timer_t* handle) {
      auto cb = (TimerCb*)handle->data;
      cb->invoke();
    }, 0, repeat);

    return (int)timer;
  }

  void clearInterval(int timer) {
    auto handle = (uv_timer_t *)timer;
    auto fTimer = std::find(vecTimer.begin(), vecTimer.end(), handle);
    if (fTimer != vecTimer.end()) {
      uv_timer_stop(handle);
      delete (TimerCb*)(handle->data);
      delete handle;
      vecTimer.erase(fTimer);
    }
  }

}
