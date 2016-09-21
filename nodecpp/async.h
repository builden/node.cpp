#pragma once
#include "nodecpp-def.h"
#include "error.h"
#include "singleton.h"

namespace nodecpp {
  using AsyncCb_t = function<void(const Error&, json&)>;
  using AsyncFuncArr_t = vector<function<void(AsyncCb_t)>>;
  using AsyncResultCb_t = function<void(const Error&, const vector<json>&)>;

  class Async : public Singleton<Async> {
  public:
    void series(AsyncFuncArr_t& funcs, AsyncResultCb_t cb);
    void parallel(AsyncFuncArr_t& funcs, AsyncResultCb_t cb);
  };

  extern Async& async;
}