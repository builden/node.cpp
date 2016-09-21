#include "async.h"

namespace nodecpp {
  Async& async = Async::instance();

  void Async::series(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
    size_t len = funcs.size();
    for (size_t i = 0; i < len; ++i) {
      auto func = funcs[i];
      func([cb, i, len](const Error& err, json& j) {
        cb(err, { R"({"a": "123"})"_json });
      });
    }
  }

  void Async::parallel(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
    size_t len = funcs.size();
    for (size_t i = 0; i < len; ++i) {
      auto func = funcs[i];
      func([cb, i, len](const Error& err, json& j) {
        cb(err, { R"({"a": "123"})"_json });
      });
    }
  }

}