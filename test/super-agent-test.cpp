#include "stdafx.h"
#include "super-agent-test.h"
#include <nodecpp/super-agent.h>

TEST_F(SuperAgentTest, get) {
  // sa.get("http://localhost:3000/abc/dad?que=xx&ad=xddx#hash")
  // sa.get("http://ip.qq.com")
  sa.get("http://dir.minigame.qq.com/cgi-bin/QQGameOpen/qqgame_comm_count_opr?prefix=mod&fieldname=subscribe&id=1&gameid=30012&opr=0")
    .set("User-Agent", "haha")
    .set("www", "xx")
    .setCookie(fmt::format("uin=o{}", 149911602))
    .setCookie("skey=@W2TQzVhHi")
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
    cout << res.data.toString() << endl;
    // fs.writeFileSync("D:/ip.html", res.data);
  });

/* // 需要先开启http服务
  sa.get("http://localhost:8080/notexist.txt")
    .end([](const Error& err, const ServerResponse& res) {
    EXPECT_TRUE(err);
    EXPECT_EQ(s.trim(err.str()), "Not Found");
    EXPECT_EQ(res.statusCode, 404);
    });*/

  // 服务器没有启动，无法建立连接
  sa.get("http://localhost:9999/notexist.txt")
      .end([](const Error& err, const ServerResponse& res) {
      EXPECT_TRUE(err);
      EXPECT_EQ(s.trim(err.str()), "RequestWrap HttpSendRequestA Error 12029");
      EXPECT_EQ(res.statusCode, 0);
    });

  // 无效域名
  sa.get("http://www.notexistss.com/notexist.txt")
    .end([](const Error& err, const ServerResponse& res) {
    EXPECT_TRUE(err);
    EXPECT_EQ(s.trim(err.str()), "Gateway Time-out");
    EXPECT_EQ(res.statusCode, 504);
  });

/*
  sa.post("http://localhost:3000/abc/dad?que=xx&ad=xddx")
    .accept("application/x-www-form-urlencoded")
    .send(R"({"a": "中文a"})"_json)
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