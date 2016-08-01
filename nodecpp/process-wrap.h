#pragma once
#include "nodecpp-def.h"
#include "handle-wrap.h"

namespace nodecpp {
  using ExitCb_t = function<void(int64_t, const string&)>;
  class ProcessWrap : public HandleWrap {
  public:
    ProcessWrap();
    ~ProcessWrap();

    int spawn(json options);

    ExitCb_t exitCb = nullptr;
    int pid = 0;

  private:
    static void OnExit(uv_process_t* handle, int64_t exit_status, int term_signal);
  private:
    uv_process_t process_;
  };
}