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

  auto j4 = json::parse(R"({"a":"b","c":123})");
  EXPECT_EQ(j3, j4);
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

TEST_F(NlohmannJsonTest, arr) {
  json j;
  j.push_back("foo");
  j.push_back(1);
  j.push_back(true);

  EXPECT_EQ(j.dump(), R"(["foo",1,true])");
}

TEST_F(NlohmannJsonTest, exception) {
  json j;
  try {
    j = json::parse("a");
  }
  catch (const std::exception & e) {
    cout << e.what() << endl;
  }
}

TEST_F(NlohmannJsonTest, exceptionTest) {
  json j;
  EXPECT_TRUE(j.is_null());
  EXPECT_FALSE(j.is_object());
  json sub = j["sub"];
  EXPECT_TRUE(sub.is_null());
  EXPECT_FALSE(sub.is_number());
  EXPECT_FALSE(sub.is_string());
  
  j = R"({ "sub": 123 })"_json;
  EXPECT_TRUE(sub.is_null());

  json sub2 = j["sub"];
  EXPECT_TRUE(sub2.is_number());
  EXPECT_FALSE(sub2.is_null());
  EXPECT_FALSE(sub2.is_string());

  j = R"({ "sub": ["abc", "cbd"] })"_json;
  json sub3 = j["sub"];
  EXPECT_TRUE(sub3.is_array());
  for (const auto& item : sub3) {
    EXPECT_TRUE(item.is_string());
    cout << item.get<string>() << endl;
  }

  const json& sub4 = j["sub"];
  EXPECT_TRUE(sub4.is_array());
  for (const auto& item : sub4) {
    EXPECT_TRUE(item.is_string());
    cout << item.get<string>() << endl;
  }

  j = R"({ "sub": 123 })"_json;
  // assert
  // EXPECT_TRUE(sub4.is_array());


  const json& sub5 = j["xxx"];
  EXPECT_TRUE(sub5.is_null());
}

TEST_F(NlohmannJsonTest, reference) {
  json j;
  j["sub"] = 1;
  EXPECT_EQ(j.dump(), R"({"sub":1})");

  json sub = j["sub"];
  sub = 2;
  EXPECT_EQ(sub.dump(), "2");
  EXPECT_EQ(j.dump(), R"({"sub":1})");

  json& refsub = j["sub"];
  refsub = 3;
  EXPECT_EQ(refsub.dump(), "3");
  EXPECT_EQ(j.dump(), R"({"sub":3})");
  string key = "sub";
  EXPECT_EQ(j[key].dump(), "3");
}