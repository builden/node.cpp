#include "timers.h"
#include <uv.h>
#include "timer-wrap.h"

namespace nodecpp {
  using TimerWrapVec = std::vector<TimerWrap*>;
  TimerWrapVec vecTimer;

  int setTimeout(TimerCb_t cb, int after) {
    auto timer = new TimerWrap();
    auto err = timer->start(after, 0, cb);
    if (err) {
      delete timer;
      return err;
    }
    vecTimer.emplace_back(timer);
    
    return (int)timer;
  }

  void clearTimeout(int timer) {
    auto wrap = (TimerWrap *)timer;
    
    auto fTimer = std::find(vecTimer.begin(), vecTimer.end(), wrap);
    if (fTimer != vecTimer.end()) {
      wrap->stop();
      delete wrap;
      vecTimer.erase(fTimer);
    }
  }

  int setInterval(TimerCb_t cb, int repeat) {
    auto timer = new TimerWrap();
    auto err = timer->start(0, repeat, cb);
    if (err) {
      delete timer;
      return err;
    }
    vecTimer.emplace_back(timer);

    return (int)timer;
  }

  void clearInterval(int timer) {
    auto wrap = (TimerWrap *)timer;

    auto fTimer = std::find(vecTimer.begin(), vecTimer.end(), wrap);
    if (fTimer != vecTimer.end()) {
      wrap->stop();
      delete wrap;
      vecTimer.erase(fTimer);
    }
  }

}
