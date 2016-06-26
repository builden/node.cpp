#include "stdafx.h"
#include "regex-test.h"
#include <nodecpp/std-regex-ex.h>

TEST_F(RegexTest, match) {
  std::regex portPattern(R"(:[0-9]*$)");
  EXPECT_TRUE(std::regex_match(":8000", portPattern));
  EXPECT_FALSE(std::regex_match("8000", portPattern));
  EXPECT_FALSE(std::regex_match(":8000ab", portPattern));
}

TEST_F(RegexTest, search) {
  std::regex protocolPattern("^([a-z0-9.+-]+:)", std::regex_constants::icase);
  std::smatch protoMatch;
  string str = "http://www.qq.com";
  std::regex_search(str, protoMatch, protocolPattern);
  EXPECT_FALSE(protoMatch.empty());
  EXPECT_EQ(protoMatch.size(), 2);
  EXPECT_EQ(protoMatch.str(0), "http:");
  EXPECT_EQ(protoMatch.str(1), "http:");
}

TEST_F(RegexTest, replace) {
  string str = "D:/1/2/3";
  string replaceToStr = "\\";
  std::regex replacePattern(R"(/)", std::regex_constants::icase);
  string result = std::regex_replace(str, replacePattern, replaceToStr);
  EXPECT_EQ(result, R"(D:\1\2\3)");
}

TEST_F(RegexTest, replace_ex) {
  regex re(R"((YYYY|YY|MM|DD|HH|mm|ss|SSS))");
  string rst = regex_replace("YYYY-MM-DD HH:mm:ss.SSS", re, [](const smatch& m) {
    string matched = m[1].str();
    if (matched == "YYYY") return "1970";
    else if (matched == "YYYY") return "70";
    else if (matched == "MM") return "12";
    else if (matched == "DD") return "01";
    else if (matched == "HH") return "01";
    else if (matched == "mm") return "02";
    else if (matched == "ss") return "03";
    else if (matched == "SSS") return "001";
    else return matched.c_str();
  });

  EXPECT_EQ(rst, "1970-12-01 01:02:03.001");
}