#include "stdafx.h"
#include "querystring-test.h"

TEST_F(QuerystringTest, parse) {
  Json j = qs.parse("foo=bar&baz=qux&baz=quux&corge");
  EXPECT_EQ(j["foo"].string_value(), "bar");
  EXPECT_EQ(j["corge"].string_value(), "");
  EXPECT_EQ(j["baz"].dump(), "[\"qux\", \"quux\"]");
}

TEST_F(QuerystringTest, stringify) {
  Json::array arr({ "qux", "quux" });
  Json j = Json::object{
    { "foo", "bar" },
    { "baz", arr },
    { "corge", "" },
    { "bool", false },
    { "num", 1.2 },
    { "chs", "ÖÐÎÄ" }
  };

  EXPECT_EQ(qs.stringify(j), "baz=qux&baz=quux&bool=false&chs=%E4%B8%AD%E6%96%87&corge=&foo=bar&num=1.2");
}