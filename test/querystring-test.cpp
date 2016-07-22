#include "stdafx.h"
#include "querystring-test.h"

TEST_F(QuerystringTest, parse) {
  json& j = qs.parse("foo=bar&baz=qux&baz=quux&corge");
  EXPECT_EQ(j["foo"].get<string>(), "bar");
  EXPECT_EQ(j["corge"].get<string>(), "");
  EXPECT_EQ(j["baz"].dump(), "[\"qux\",\"quux\"]");
}

TEST_F(QuerystringTest, stringify) {
  json arr({ "qux", "quux" });
  json j = {
    { "foo", "bar" },
    { "baz", arr },
    { "corge", "" },
    { "bool", false },
    { "num", 1.2 },
    { "chs", "ÖÐÎÄ" }
  };

  EXPECT_EQ(qs.stringify(j), "baz=qux&baz=quux&bool=false&chs=%E4%B8%AD%E6%96%87&corge=&foo=bar&num=1.2");
}