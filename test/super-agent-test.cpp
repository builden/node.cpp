#include "stdafx.h"
#include "super-agent-test.h"
#include <nodecpp/super-agent.h>

TEST_F(SuperAgentTest, get) {
  sa.get("http://ip.qq.com")
    .end([](const Error& err, const ServerResponse& res) {
    EXPECT_FALSE(err);
    EXPECT_GT(res.data.size(), 1u);
    // fs.writeFileSync("D:/ip.html", res.data);
    cout << "at get response" << endl;
  });

  run();
}