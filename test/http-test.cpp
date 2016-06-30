#include "stdafx.h"
#include "http-test.h"

TEST_F(HttpTest, get) {
  http
    .get("http://ip.qq.com", [](const ServerResponse& res) {
      cout << "on res" << endl;
    });

  EXPECT_EQ(http.METHODS.size(), 33);
  EXPECT_EQ(http.STATUS_CODES.size(), 62);

  run();
}