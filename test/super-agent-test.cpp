#include "stdafx.h"
#include "super-agent-test.h"
#include <nodecpp/super-agent.h>

TEST_F(SuperAgentTest, get) {
  // sa.get("http://localhost:3000/abc/dad?que=xx&ad=xddx#hash")
  sa.get("http://ip.qq.com")
    .set("User-Agent", "haha")
    .set("www", "xx")
    .onProgress([](uint32_t currByte, uint32_t totalByte) {
      cout << "progress: " << currByte << "/" << totalByte << endl;
    })
    .end([](const Error& err, const ServerResponse& res) {
    EXPECT_FALSE(err);
    EXPECT_GT(res.data.size(), 1u);
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusMessage, "OK");
    for (auto& header : res.headers) {
      cout << header.first << ": " << header.second << endl;
    }
    // fs.writeFileSync("D:/ip.html", res.data);
  });

/*
  sa.post("http://localhost:3000/abc/dad?que=xx&ad=xddx")
    .accept("application/x-www-form-urlencoded")
    .send(R"({"a": "ÖÐÎÄa"})"_json)
    .end([](const Error& err, const ServerResponse& res) {
    EXPECT_FALSE(err);
    EXPECT_GT(res.data.size(), 1u);
    EXPECT_EQ(res.statusCode, 200);
    EXPECT_EQ(res.statusMessage, "OK");
    for (auto& header : res.headers) {
      cout << header.first << ": " << header.second << endl;
    }
    fs.writeFileSync("D:/ip.html", res.data);
  });*/

  nodecpp::run();
}