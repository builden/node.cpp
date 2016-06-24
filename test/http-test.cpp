#include "stdafx.h"
#include "http-test.h"

TEST_F(HttpTest, get) {
  http
    .get("http://ip.qq.com", [](const ServerResponse& res) {
      cout << "on res" << endl;
    });

  run();
}