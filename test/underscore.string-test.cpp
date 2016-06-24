#include "stdafx.h"
#include "underscore.string-test.h"
#include <regex>

TEST_F(UnderscoreStringTest, slice) {
  EXPECT_EQ(s.slice("abcd", 0), "abcd");
  EXPECT_EQ(s.slice("abcd", 1), "bcd");
  EXPECT_EQ(s.slice("abcd", 5), "");
  EXPECT_EQ(s.slice("abcd", -3), "bcd");
  EXPECT_EQ(s.slice("abcd", -4), "abcd");
  EXPECT_EQ(s.slice("abcd", -5), "abcd");
  EXPECT_EQ(s.slice("abcd", 0, -1), "abc");
  EXPECT_EQ(s.slice("abcd", 1, -2), "b");
  EXPECT_EQ(s.slice("abcd", 0, -4), "");
  EXPECT_EQ(s.slice("abcd", 1, 1), "");
  EXPECT_EQ(s.slice("abcd", 1, 0), "");
  EXPECT_EQ(s.slice("abcd", -3, -1), "bc");
}

TEST_F(UnderscoreStringTest, split) {
  svec_t sv = s.split("abc -cdb -ddd", " -", 2);
  EXPECT_EQ(sv.size(), 2);
  EXPECT_EQ(sv[0], "abc");
  EXPECT_EQ(sv[1], "cdb");

  svec_t sv2 = s.split("abcde", " ");
  EXPECT_EQ(sv2.size(), 1);
  EXPECT_EQ(sv2[0], "abcde");
}

TEST_F(UnderscoreStringTest, join) {
  svec_t sv = { "abc", "cde", "efg" };
  EXPECT_EQ(s.join(sv), "abccdeefg");
  EXPECT_EQ(s.join(sv, ","), "abc,cde,efg");
}

TEST_F(UnderscoreStringTest, startsWith) {
  EXPECT_TRUE(s.startsWith("abcde", "abc"));
  EXPECT_TRUE(s.startsWith("abcde", "bc", 1));
  EXPECT_FALSE(s.startsWith("abcde", "abc", 1));
  EXPECT_FALSE(s.startsWith("abcde", "abc", 10));
}

TEST_F(UnderscoreStringTest, endsWith) {
  EXPECT_TRUE(s.endsWith("abcde", "cde"));
  EXPECT_TRUE(s.endsWith("abcde", "cd", 4));
  EXPECT_FALSE(s.endsWith("abcde", "cde", 4));
  EXPECT_TRUE(s.endsWith("abcde", "cde", 10));
}

TEST_F(UnderscoreStringTest, trim) {
  EXPECT_EQ(s.ltrim(" \t\n-acd-\n\t  ", " \t\n-"), "acd-\n\t  ");
  EXPECT_EQ(s.rtrim(" \t\n-acd-\n\t  ", " \t\n-"), " \t\n-acd");
  EXPECT_EQ(s.trim(" \t\n-acd-\n\t  ", " \t\n-"), "acd");
  EXPECT_EQ(s.ltrim(" \t\n- acd-\n\t  "), "- acd-\n\t  ");
}

TEST_F(UnderscoreStringTest, repeat) {
  EXPECT_EQ(s.repeat("abc", 3), "abcabcabc");
  EXPECT_EQ(s.repeat("a", 0), "");
  EXPECT_EQ(s.repeat("a", -2), "");
}

TEST_F(UnderscoreStringTest, toLower_toUpper) {
  EXPECT_EQ(s.toLower("Hello Nodepp"), "hello nodepp");
  EXPECT_EQ(s.toUpper("Hello Nodepp"), "HELLO NODEPP");
}

TEST_F(UnderscoreStringTest, pad) {
  EXPECT_EQ(s.pad("abcd", 8), "  abcd  ");
  EXPECT_EQ(s.lpad("abcd", 8), "    abcd");
  EXPECT_EQ(s.rpad("abcd", 8), "abcd    ");

  EXPECT_EQ(s.pad("abcd", 8, "0"), "00abcd00");
  EXPECT_EQ(s.pad("abcd", 7, "0"), "00abcd0");

  EXPECT_EQ(s.pad("abcd", 4), "abcd");
  EXPECT_EQ(s.pad("abcd", 3), "abcd");
}

TEST_F(UnderscoreStringTest, strLeft_strRight) {
  EXPECT_EQ(s.strLeft("This_is_a_test_string", "_"), "This");
  EXPECT_EQ(s.strLeft("abcdef", "cdf"), "abcdef");
  EXPECT_EQ(s.strRight("This_is_a_test_string", "_"), "is_a_test_string");
  EXPECT_EQ(s.strRight("abcdef", "cdf"), "abcdef");

  EXPECT_EQ(s.strLeftBack("This_is_a_test_string", "_"), "This_is_a_test");
  EXPECT_EQ(s.strLeftBack("abcdef", "cdf"), "abcdef");

  EXPECT_EQ(s.strRightBack("This_is_a_test_string", "_"), "string");
  EXPECT_EQ(s.strRightBack("abcdef", "cdf"), "abcdef");
}

TEST_F(UnderscoreStringTest, indexOf_lastIndexOf) {
  EXPECT_EQ(s.indexOf("abcdefcd", "cd"), 2);
  EXPECT_EQ(s.indexOf("abcdefcd", "cdf"), -1);

  EXPECT_EQ(s.lastIndexOf("abcdefcd", "cd"), 6);
  EXPECT_EQ(s.lastIndexOf("abcdefcd", "cdf"), -1);
}

TEST_F(UnderscoreStringTest, includes) {
  EXPECT_TRUE(s.includes("Hello World", "ello"));
  EXPECT_FALSE(s.includes("Hello World", "ello", 2));
  EXPECT_FALSE(s.includes("Hello World", "hello"));
}

TEST_F(UnderscoreStringTest, truncate) {
  EXPECT_EQ(s.truncate("abcdefg", 5), "abcde...");
  EXPECT_EQ(s.truncate("abcdefg", 7), "abcdefg");
}

TEST_F(UnderscoreStringTest, surround) {
  EXPECT_EQ(s.surround("abc", "eee"), "eeeabceee");
  EXPECT_EQ(s.surround("abc", "\""), "\"abc\"");
}

TEST_F(UnderscoreStringTest, quote) {
  EXPECT_EQ(s.quote("abc"), "\"abc\"");
  EXPECT_EQ(s.quote("abc", '\''), "'abc'");

  EXPECT_EQ(s.unquote("\"abc\""), "abc");
  EXPECT_EQ(s.unquote("\'abc\'", '\''), "abc");
}

TEST_F(UnderscoreStringTest, replace) {
  EXPECT_EQ(s.replace("abcda", "a", "ef"), "efbcda");
  EXPECT_EQ(s.replace("abcda abcda", "abc", "ef"), "efda abcda");
  EXPECT_EQ(s.replace("abcdA", "A", "ef"), "abcdef");
  EXPECT_EQ(s.replace("abcdA", "A", "ef", true), "efbcdA");

  EXPECT_EQ(s.replace("D:\\a\\b", "\\", "/"), "D:/a\\b");
}

TEST_F(UnderscoreStringTest, replaceAll) {
  EXPECT_EQ(s.replaceAll("abcda", "a", "ef"), "efbcdef");
  EXPECT_EQ(s.replaceAll("abcda abcda", "abc", "ef"), "efda efda");
  EXPECT_EQ(s.replaceAll("abcdA", "A", "ef"), "abcdef");
  EXPECT_EQ(s.replaceAll("abCDA", "A", "ef", true), "efbCDef");

  EXPECT_EQ(s.replaceAll("D:\\a\\b", "\\", "/"), "D:/a/b");
}

TEST_F(UnderscoreStringTest, reverse) {
  EXPECT_EQ(s.reverse("abcde"), "edcba");
  EXPECT_EQ(s.reverse("a中b文c"), "c文b中a");
}

TEST_F(UnderscoreStringTest, regex) {
  std::string fnames[] = { "foo.txt", "bar.txt", "baz.dat", "zoidberg" };
  std::regex txt_regex("[a-z]+\\.txt");

  for (const auto &fname : fnames) {
    std::cout << fname << ": " << std::regex_match(fname, txt_regex) << '\n';
  }
}

TEST_F(UnderscoreStringTest, capitalize_decapitalize_camelize) {
  EXPECT_EQ(s.decapitalize("Foo Bar"), "foo Bar");
  EXPECT_EQ(s.decapitalize(" Foo Bar"), " Foo Bar");

  EXPECT_EQ(s.capitalize("foo Bar"), "Foo Bar");
  EXPECT_EQ(s.capitalize("FOO Bar", true), "Foo bar");

  EXPECT_EQ(s.camelize("moz-transform"), "mozTransform");
  EXPECT_EQ(s.camelize("--moz-transform"), "MozTransform");
  EXPECT_EQ(s.camelize("_moz_transform"), "MozTransform");
  EXPECT_EQ(s.camelize("Moz-transform"), "MozTransform");
  EXPECT_EQ(s.camelize("-_moz-transform", true), "mozTransform");
}