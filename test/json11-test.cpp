#include "stdafx.h"
#include "json11-test.h"

TEST_F(Json11Test, constructor) {
  class Point {
  public:
    int x;
    int y;
    Point(int x, int y) : x(x), y(y) {}
    Json to_json() const { return Json::array { x, y }; }
  };

  std::vector<Point> points = { { 1, 2 }, { 10, 20 }, { 100, 200 } };
  std::string points_json = Json(points).dump();
  std::cout << points_json << std::endl;

  Json::array arr = { 1, 2, 3 };
  Json my_json = Json::object{
    { "key1", "value1中文" },
    { "key2", false },
    { "key3",  arr}, // 不支持直接嵌套
  };
  std::cout << my_json["key1"].string_value() << std::endl;
  string json_str = my_json.dump();
  std::cout << json_str << std::endl;

  // 数组操作
  Json json = Json::array { Json::object{ { "k", "v" } }, Json::object{ { "k", "v2" } } };
  std::string str = json[1]["k"].string_value();
  std::cout << str << std::endl;
}

TEST_F(Json11Test, parse) {
  const string simple_test =
    R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null], "k4": 4.5, "k5": true})";

  string err;
  auto json = Json::parse(simple_test, err);

  EXPECT_EQ(json["k1"].string_value(), "v1");
  EXPECT_EQ(json["k2"].int_value(), 42);
  EXPECT_EQ(json["k4"].number_value(), 4.5);
  EXPECT_TRUE(json["k5"].bool_value());

  std::cout << "k3: " << json["k3"].dump() << "\n";
  EXPECT_EQ(json["k3"].array_items().size(), 5);
  for (auto &k : json["k3"].array_items()) {
    std::cout << "    - " << k.dump() << "\n";
  }

  const string obj_arr =
    R"([
  {"left": 0, "top": 0, "width": 100, "height": 100},
  {"left": 0, "top": 0, "width": 200, "height": 200}
])";

  auto json_arr = Json::parse(obj_arr, err);
  auto arr = json_arr.array_items();
  for (uint32_t i = 0; i < arr.size(); i++) {
    Json& j = arr[i];
    if (i == 0) {
      EXPECT_EQ(j["width"].number_value(), 100);
    }
    else if (i == 1) {
      EXPECT_EQ(j["width"].number_value(), 200);
    }
  }

  for (auto &item : arr) {
    cout << " ==== " << item["height"].number_value() << endl;
  }
}

TEST_F(Json11Test, test_comment) {
  const string comment_test = R"({
      // comment /* with nested comment */
      "a": 1,
      // comment
      // continued
      "b": "text",
      /* multi
         line
         comment */
      // and single-line comment
      "c": [1, 2, 3]
    })";

  string err_comment;
  auto json_comment = Json::parse(
    comment_test, err_comment, JsonParse::COMMENTS);
  if (!err_comment.empty()) {
    std::cout << err_comment << std::endl;
  }
  else {
    string expectResult = R"({"a": 1, "b": "text", "c": [1, 2, 3]})";
    EXPECT_EQ(json_comment.dump(), expectResult);
  }
}

TEST_F(Json11Test, bad_comment) {
  string failing_comment_test = R"({
      /* bad comment
      "a": 1,
    })";

  string err_failing_comment;
  auto json_failing_comment = Json::parse(
    failing_comment_test, err_failing_comment, JsonParse::COMMENTS);
  if (!err_failing_comment.empty()) {
    EXPECT_EQ(err_failing_comment, "unexpected end of input inside multi-line comment");
  }
  else {
    printf("Result: %s\n", json_failing_comment.dump().c_str());
  }
}

TEST_F(Json11Test, arr_constructor) {
  std::list<int> l1{ 1, 2, 3 };
  std::vector<int> l2{ 1, 2, 3 };
  std::set<int> l3{ 1, 2, 3 };
  EXPECT_EQ(Json(l1), Json(l2));
  EXPECT_EQ(Json(l2), Json(l3));
}

#include <unordered_map>
TEST_F(Json11Test, obj_constructor) {
  std::map<string, string> m1{ { "k1", "v1" }, { "k2", "v2" } };
  std::unordered_map<string, string> m2{ { "k1", "v1" }, { "k2", "v2" } };
  EXPECT_EQ(Json(m1), Json(m2));
}

TEST_F(Json11Test, modify) {
  Json::array arr = { 1, "str", true };
  Json::object obj = {
    { "k1", "value1中文" },
    { "k2", 13 },
  };
  obj["k3"] = 12;
  Json my_json = obj;
  string expectMyJson = R"({"k1": "value1中文", "k2": 13, "k3": 12})";
  EXPECT_EQ(my_json.dump(), expectMyJson);

  obj["k4"] = 64;
  obj["k5"] = arr;
  arr[0] = "modified"; // 这里对obj不会产生影响

  Json my_json2 = obj;
  string expectMyJson2 = R"({"k1": "value1中文", "k2": 13, "k3": 12, "k4": 64, "k5": [1, "str", true]})";
  EXPECT_EQ(my_json2.dump(), expectMyJson2);
}

TEST_F(Json11Test, arr) {
  vector<wstring> m_vecFuncs{ L"abc", L"ced" };
  Json::array arr;
  for (auto& func : m_vecFuncs) {
    arr.emplace_back(Json::object{ { "func", iconv.wstrToStr(func) } });
  }

  Json j = arr;
  EXPECT_EQ(j.dump(), R"([{"func": "abc"}, {"func": "ced"}])");

  string err;
  Json afterJson = Json::parse(j.dump(), err);
  Json::array afterArr = afterJson.array_items();
  for (auto& item : afterArr) {
    Json::object obj = item.object_items();
    cout << obj["func"].string_value() << endl;
  }
}