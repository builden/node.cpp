#include "stdafx.h"
#include "regex-test.h"
#include <regex>

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