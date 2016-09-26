#include "async.h"

namespace nodecpp {
  Async& async = Async::instance();

  class AsyncInner {
  public:
    void series(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
      funcs_ = funcs;
      cb_ = cb;
      funcCount_ = funcs.size();
      results_.resize(funcCount_);

      runSeriesFuncs();
    }

    void runSeriesFuncs() {
      auto func = funcs_[currIdx_];
      func([this](const Error& err, json& j) {
        if (err) return cb_(err, results_);
        results_[currIdx_] = j;
        if (++currIdx_ == funcCount_) return cb_(err, results_);
        runSeriesFuncs();
      });
    }

  private:
    size_t currIdx_ = 0;
    size_t funcCount_ = 0;
    AsyncFuncArr_t funcs_;
    AsyncResultCb_t cb_;
    vector<json> results_;
  };

  void Async::series(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
    auto inner = new AsyncInner();
    inner->series(funcs, [inner, cb](const Error& err, const vector<json>& results) {
      cb(err, results);
      delete inner;
    });
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