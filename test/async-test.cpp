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
}

TEST_F(AsyncTest, series) {
  async.series<json>(AsyncJsonFuncArr_t{
    [](AsyncJsonCb_t cb) {
      cout << "series 1: " << moment().format() << endl;
      setTimeout([=] {
        cout << "series 2: " << moment().format() << endl;
        cb(Error(0), R"({ "a": 1 })"_json);
      }, 100);
    },
    [](AsyncJsonCb_t cb) {
      cout << "series 3: " << moment().format() << endl;
      setTimeout([=] {
        cout << "series 4: " << moment().format() << endl;
        cb(Error("p2"), R"({ "a": "b" })"_json);
      }, 200);
    },
    [](AsyncJsonCb_t cb) {
      cout << "series 5: " << moment().format() << endl;
      setTimeout([=] {
        cout << "series 6: " << moment().format() << endl;
        cb(Error(0), R"({ "a": "c" })"_json);
      }, 100);
    }
  }, [](const Error& err, const vector<json>& results) {
    cout << "series 10: " << moment().format() << endl;
    if (err) cout << "series err: " << err.str() << endl;
    for (auto& result : results) {
      cout << result.dump() << endl;
    }
  });

  async.series<int>(AsyncIntFuncArr_t{
    [](AsyncIntCb_t cb) {
      cout << "series int 1: " << moment().format() << endl;
      setTimeout([=] {
        cout << "series int 2: " << moment().format() << endl;
        cb(Error(0), 1);
      }, 100);
    },
    [](AsyncIntCb_t cb) {
      cout << "series int 3: " << moment().format() << endl;
      setTimeout([=] {
        cout << "series int 4: " << moment().format() << endl;
        cb(Error(0), 2);
      }, 200);
    },
    [](AsyncIntCb_t cb) {
      cout << "series int 5: " << moment().format() << endl;
      setTimeout([=] {
        cout << "series int 6: " << moment().format() << endl;
        cb(Error(0), 3);
      }, 100);
    }
  }, [](const Error& err, const ivec_t& results) {
    cout << "series int 10: " << moment().format() << endl;
    if (err) cout << "series int err: " << err.str() << endl;
    for (auto& result : results) {
      cout << result << endl;
    }
  });

  run();
}

TEST_F(AsyncTest, parallel) {
  async.parallel<json>(AsyncJsonFuncArr_t{
    [](AsyncJsonCb_t cb) {
      cout << "parallel 1: " << moment().format() << endl;
      setTimeout([=] {
        cout << "parallel 2: " << moment().format() << endl;
        cb(Error(0), json::object());
      }, 200);
    },
    [](AsyncJsonCb_t cb) {
      cout << "parallel 3: " << moment().format() << endl;
      setTimeout([=] {
        cout << "parallel 4: " << moment().format() << endl;
        cb(Error("p3"), R"({ "a": "b" })"_json);
      }, 150);
    },
    [](AsyncJsonCb_t cb) {
      cout << "parallel 5: " << moment().format() << endl;
      setTimeout([=] {
        cout << "parallel 6: " << moment().format() << endl;
        cb(Error(0), R"({ "a": "c" })"_json);
      }, 100);
    }
  }, [](const Error& err, const vector<json>& results) {
    cout << "parallel 10: " << moment().format() << endl;
    if (err) cout << "parallel err: " << err.str() << endl;
    for (auto& result : results) {
      cout << result.dump() << endl;
    }
  });

  run();
}

TEST_F(AsyncTest, mapLimit) {
  vector<int> arr = { 2, 4, 1, 3, 1 };
  async.mapLimit<int, int>(arr, 2, [](int item, function<void(const Error&, int)> cb) {
    cout << "in mapLimit func beg " << item << "; " << moment().format() << endl;
    setTimeout([cb, item]() {
      cout << "in mapLimit func end " << item << "; " << moment().format() << endl;
      if (3 == item) cb(Error("dd"), item);
      else cb(Error(), item);
    }, item * 10);
  }, [](const Error& err, const vector<int>& results) {
    cout << "mapLimit end " << (err) << "; " << moment().format() << endl;
    for (auto i : results) {
      cout << "mapLimit result " << i << endl;
    }
  });
  
  run();
}