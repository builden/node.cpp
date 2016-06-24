#include "stdafx.h"
#include "url-test.h"

TEST_F(UrlTest, parse) {
  UrlObj obj = url.parse("http://user:pass@host.com:8080/p/a/t/h?query=string#hash");
  fmt::print("protocol: {}\n", obj.protocol);
  fmt::print("slashes: {}\n", obj.slashes);
  fmt::print("auth: {}\n", obj.auth);
  fmt::print("host: {}\n", obj.host);
  fmt::print("port: {}\n", obj.port);
  fmt::print("hostname: {}\n", obj.hostname);
  fmt::print("hash: {}\n", obj.hash);
  fmt::print("search: {}\n", obj.search);
  fmt::print("query: {}\n", obj.query);
  fmt::print("pathname: {}\n", obj.pathname);
  fmt::print("path: {}\n", obj.path);
  fmt::print("href: {}\n", obj.href);
}

TEST_F(UrlTest, format) {

}

TEST_F(UrlTest, resolve) {

}