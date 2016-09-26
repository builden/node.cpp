#include "async.h"

namespace nodecpp {
  Async& async = Async::instance();

  class SeriesInner {
  public:
    void series(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
      funcs_ = funcs;
      cb_ = cb;
      funcCount_ = funcs.size();

      runSeriesFuncs();
    }

    void runSeriesFuncs() {
      auto func = funcs_[currIdx_];
      func([this](const Error& err, json& j) {
        if (err) return cb_(err, results_);
        results_.emplace_back(j);
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
    auto inner = new SeriesInner();
    inner->series(funcs, [inner, cb](const Error& err, const vector<json>& results) {
      cb(err, results);
      delete inner;
    });
  }

  class ParallelInner {
  public:
    void parallel(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
      funcs_ = funcs;
      cb_ = cb;
      funcCount_ = funcs.size();
      results_.resize(funcCount_);

      for (size_t i = 0; i < funcCount_; ++i) {
        auto func = funcs[i];
        func([i, this](const Error& err, json& j) {
          ++completedCount_;
          if (completedCount_ == funcCount_) {
            if (!haveErrorRst_) {
              if (!err) results_[i] = j;
              cb_(err, results_);
            }
            delete this;
          }
          else {
            if (haveErrorRst_) return;
            if (err) {
              cb_(err, results_);
              haveErrorRst_ = true;
            }
            else {
              results_[i] = j;
            }
          }
        });
      }
    }

  private:
    size_t completedCount_ = 0;
    size_t funcCount_ = 0;
    AsyncFuncArr_t funcs_;
    AsyncResultCb_t cb_;
    vector<json> results_;
    bool haveErrorRst_ = false;
  };

  void Async::parallel(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
    auto inner = new ParallelInner();
    inner->parallel(funcs, cb);
  }

}