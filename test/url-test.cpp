#include "stdafx.h"
#include "url-test.h"

TEST_F(UrlTest, parse) {
  // note: hash字段不会发送到服务器，这个一般只用于请求最新资源，防止同名文件缓存
  UrlObj obj = url.parse("http://user:pass@host.com:8080/p/a/t/h?query=string#hash");
  EXPECT_EQ(obj.protocol, "http:");
  EXPECT_EQ(obj.slashes, true);
  EXPECT_EQ(obj.auth, "user:pass");
  EXPECT_EQ(obj.host, "host.com:8080");
  EXPECT_EQ(obj.port, "8080");
  EXPECT_EQ(obj.hostname, "host.com");
  EXPECT_EQ(obj.hash, "#hash");
  EXPECT_EQ(obj.search, "?query=string");
  EXPECT_EQ(obj.query, "query=string");
  EXPECT_EQ(obj.pathname, "/p/a/t/h");
  EXPECT_EQ(obj.path, "/p/a/t/h?query=string");
  EXPECT_EQ(obj.href, "http://user:pass@host.com:8080/p/a/t/h?query=string#hash");

  UrlObj obj2 = url.parse("http://ip.qq.com");
  EXPECT_EQ(obj2.port, "");
}

TEST_F(UrlTest, format) {

}

TEST_F(UrlTest, resolve) {

}