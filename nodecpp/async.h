#pragma once
#include "nodecpp-def.h"
#include "error.h"
#include "singleton.h"

namespace nodecpp {
  using AsyncJsonCb_t = function<void(const Error&, const json&)>;
  using AsyncJsonFuncArr_t = vector<function<void(AsyncJsonCb_t)>>;
  using AsyncJsonResultCb_t = function<void(const Error&, const vector<json>&)>;

  using AsyncStrCb_t = function<void(const Error&, const string&)>;
  using AsyncStrFuncArr_t = vector<function<void(AsyncStrCb_t)>>;
  using AsyncStrResultCb_t = function<void(const Error&, const svec_t&)>;

  using AsyncIntCb_t = function<void(const Error&, const int&)>;
  using AsyncIntFuncArr_t = vector<function<void(AsyncIntCb_t)>>;
  using AsyncIntResultCb_t = function<void(const Error&, const ivec_t&)>;

  template<typename T>
  class TmplSeriesInner {
  public:
    void series(vector<function<void(function<void(const Error&, const T&)>)>>& funcs,
      function<void(const Error&, const vector<T>&)> cb) {
      funcs_ = funcs;
      cb_ = cb;
      funcCount_ = funcs.size();

      runSeriesFuncs();
    }

    void runSeriesFuncs() {
      auto func = funcs_[currIdx_];
      func([this](const Error& err, const T& j) {
        if (err) return cb_(err, results_);
        results_.emplace_back(j);
        if (++currIdx_ == funcCount_) return cb_(err, results_);
        runSeriesFuncs();
      });
    }

  private:
    size_t currIdx_ = 0;
    size_t funcCount_ = 0;
    vector<function<void(function<void(const Error&, const T&)>)>> funcs_;
    function<void(const Error&, const vector<T>&)> cb_;
    vector<T> results_;
  };

  template<typename T>
  class TmplParallelInner {
  public:
    void parallel(vector<function<void(function<void(const Error&, const T&)>)>>& funcs,
      function<void(const Error&, const vector<T>&)> cb) {
      funcs_ = funcs;
      cb_ = cb;
      funcCount_ = funcs.size();
      results_.resize(funcCount_);

      for (size_t i = 0; i < funcCount_; ++i) {
        auto func = funcs[i];
        func([i, this](const Error& err, const T& j) {
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
    vector<function<void(function<void(const Error&, const T&)>)>> funcs_;
    function<void(const Error&, const vector<T>&)> cb_;
    vector<T> results_;
    bool haveErrorRst_ = false;
  };

  template<typename T, typename N>
  class TmplMapLimitInner {
  public:
    void mapLimit(const vector<N> arr, size_t limit, function<void(const N&, function<void(const Error&, const T&)>)> iteratee,
      function<void(const Error&, const vector<T>&)> cb) {
      arr_ = arr;
      iteratee_ = iteratee;
      cb_ = cb;
      size_t arrSize = arr.size();
      results_.resize(arrSize);

      size_t count = Math.min<size_t>(limit, arrSize);
      for (size_t i = 0; i < count; ++i) {
        eachOfLimit(i);
      }
    }

    void eachOfLimit(size_t idx) {
      const N& item = arr_[idx];
      iteratee_(item, [this, idx](const Error& err, const T& j) {
        ++completedCount_;
        size_t arrSize = arr_.size();
        if (completedCount_ >= arrSize) {
          if (!haveErrorRst_) {
            if (!err) results_[idx] = j;
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
            results_[idx] = j;
            if (currIdx_ < arrSize) eachOfLimit(currIdx_);
          }
        }
      });
      currIdx_++;
    }

  private:
    vector<N> arr_;
    function<void(const N&, function<void(const Error&, const T&)>)> iteratee_;
    size_t completedCount_ = 0;
    function<void(const Error&, const vector<T>&)> cb_;
    vector<T> results_;
    bool haveErrorRst_ = false;
    size_t currIdx_ = 0;
  };

  class Async : public Singleton<Async> {
  public:
    template<typename T>
    void series(vector<function<void(function<void(const Error&, const T&)>)>>& funcs, 
      function<void(const Error&, const vector<T>&)> cb) {
      auto inner = new TmplSeriesInner<T>();
      inner->series(funcs, [inner, cb](const Error& err, const vector<T>& results) {
        cb(err, results);
        delete inner;
      });
    }

    template<typename T>
    void parallel(vector<function<void(function<void(const Error&, const T&)>)>>& funcs,
      function<void(const Error&, const vector<T>&)> cb) {
      auto inner = new TmplParallelInner<T>();
      inner->parallel(funcs, cb);
    }

    template<typename T, typename N>
    void map(const vector<N> arr, function<void(const N&, function<void(const Error&, const T&)>)> iteratee,
      function<void(const Error&, const vector<T>&)> cb) {
      auto inner = new TmplMapLimitInner<T, N>();
      inner->mapLimit(arr, 0, iteratee, cb);
    }

    template<typename T, typename N>
    void mapLimit(const vector<N> arr, size_t limit, function<void(const N&, function<void(const Error&, const T&)>)> iteratee,
      function<void(const Error&, const vector<T>&)> cb) {
      auto inner = new TmplMapLimitInner<T, N>();
      inner->mapLimit(arr, limit, iteratee, cb);
    }
  };

  extern Async& async;
}