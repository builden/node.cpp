#include "stdafx.h"
#include "nlohmann-json-test.h"
#include <nodecpp/nlohmann/json.hpp>
using json = nlohmann::json;

TEST_F(NlohmannJsonTest, parse) {
  json j;
  EXPECT_EQ(j.dump(), "null");
  j["pi"] = 3.141;
  EXPECT_EQ(j.dump(), R"({"pi":3.141})");
  j["happy"] = true;
  j["name"] = "Niels";
  j["nothing"] = nullptr;
  j["answer"]["everything"] = 42;
  j["list"] = { 1, 0, 2 };
  j["object"] = { { "currency", "USD" },{ "value", 42.99 } };
  EXPECT_EQ(j.dump(), "{\"answer\":{\"everything\":42},\"happy\":true,\"list\":[1,0,2],\"name\":\"Niels\",\"nothing\":null,\"object\":{\"currency\":\"USD\",\"value\":42.99},\"pi\":3.141}");

  json j2 = {
    { "pi", 3.141 },
    { "happy", true },
    { "name", "Niels" },
    { "nothing", nullptr },
    { "answer",{
      { "everything", 42 }
    } },
    { "list",{ 1, 0, 2 } },
    { "object",{
      { "currency", "USD" },
      { "value", 42.99 }
    } }
  };

  EXPECT_EQ(j, j2);

  json emptyArray = json::array();
  EXPECT_EQ(emptyArray.dump(), "[]");
  json emptyObject = json::object();
  EXPECT_EQ(emptyObject.dump(), "{}");
  json emptyObjec2 = json({});
  EXPECT_EQ(emptyObjec2.dump(), "{}");

  json j3 = R"(
{
  "a": "b",
  "c": 123
}
)"_json;
  EXPECT_EQ(j3.dump(), R"({"a":"b","c":123})");
}

TEST_F(NlohmannJsonTest, get) {
  json j = {
    { "pi", 3.141 },
    { "happy", true },
    { "name", "Niels" },
    { "nothing", nullptr },
    { "answer",{
      { "everything", 42 }
    } },
    { "list",{ 1, false, 2 } },
    { "object",{
      { "currency", "USD" },
      { "value", 42.99 }
    } }
  };

  auto pi = j["pi"].get<double>();
  EXPECT_EQ(pi, 3.141);
  auto happy = j["happy"].get<bool>();
  EXPECT_TRUE(happy);
  EXPECT_TRUE(j["nothing"].is_null());
  auto answer = j["answer"];
  auto everything = answer["everything"].get<int>();
  EXPECT_EQ(everything, 42);

  // 遍历key value
  for (auto it = j.begin(); it != j.end(); ++it) {
    cout << it.key() << ":" << it.value() << endl;
  }

  // 遍历数组
  for (auto& item : j["list"]) {
    cout << item << endl;
  }
}

TEST_F(NlohmannJsonTest, find) {
  json j = {
    { "foo", "bar" }
  };
  EXPECT_TRUE(j.find("key") == j.end());
}

TEST_F(NlohmannJsonTest, erase) {
  json j = {
    { "foo", "bar" }
  };
  j.erase("foo");
  EXPECT_EQ(j.dump(), "{}");
}