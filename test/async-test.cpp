#include "stdafx.h"
#include "async-test.h"

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

  async.series(AsyncFuncArr_t{
    [](AsyncCb_t cb) {
      cout << "1: " << moment().format() << endl;
      setTimeout([=] {
        cout << "2: " << moment().format() << endl;
        cb(Error(0), json::object());
      }, 100);
    },
    [](AsyncCb_t cb) {
      cout << "3: " << moment().format() << endl;
      setTimeout([=] {
        cout << "4: " << moment().format() << endl;
        cb(Error(0), R"({ "a": "b" })"_json);
      }, 100);
    }
  }, [](const Error& err, const vector<json>& results) {
    cout << "5: " << moment().format() << endl;
    for (auto& result : results) {
      cout << result.dump() << endl;
    }
  });

  run();
}