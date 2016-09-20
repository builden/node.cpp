#include "stdafx.h"
#include "async-test.h"

namespace nodecpp {
  using AsyncCb_t = function<void(const Error&, json&)>;
  using AsyncFuncArr_t = vector<function<void(AsyncCb_t)>>;
  using AsyncResultCb_t = function<void(const Error&, const vector<json>&)>;
  class Async : public Singleton<Async> {
  public:
    void series(AsyncFuncArr_t& funcs, AsyncResultCb_t cb) {
      size_t len = funcs.size();
      for (size_t i = 0; i < len; ++i) {
        auto func = funcs[i];
        func([cb, i, len](const Error& err, json& j) {
          cb(err, { R"({"a": "123"})"_json });
        });
      }
    }
  };

  Async& async = Async::instance();
}


TEST_F(AsyncTest, tupleTest) {
/*
  std::tuple<int, string, bool, string> t1 = std::make_tuple(1, "a", false, "b");
  auto p0 = std::get<3>(t1);
  cout << p0 << endl;*/

  auto t2 = std::make_tuple([](int i) {
    cout << "in tuple0 i: " << i << endl;
  }, [](int i, bool b) {
    cout << "in tuble1 i " << i << "; b " << b << endl;
  });
  auto p0 = std::get<0>(t2);
  p0(2);
  auto p1 = std::get<1>(t2);
  p1(3, false);


  AsyncFuncArr_t funcs;
  funcs.emplace_back([&](AsyncCb_t cb) {
    cb(Error(0), json::object());
  });
  funcs.emplace_back([&](AsyncCb_t cb) {
    cb(Error(0), json::object());
  });

  async.series(funcs, [](const Error& err, const vector<json>& results) {
    for (auto& result : results) {
      cout << result.dump() << endl;
    }
  });
}