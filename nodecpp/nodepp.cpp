#include "nodecpp.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <uv.h>

namespace nodecpp {
  
  void run() {
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  }

  void runInterval(int interval /*= 1*/) {
    SetTimer(NULL, NULL, interval, [](HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
      uv_run(uv_default_loop(), UV_RUN_NOWAIT);
    });
  }

  void runOnce() {
    uv_run(uv_default_loop(), UV_RUN_ONCE);
  }

  void runNoWait() {
    uv_run(uv_default_loop(), UV_RUN_NOWAIT);
  }

}
