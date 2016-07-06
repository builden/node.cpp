#include "stdafx.h"
#include "iconv-test.h"

TEST_F(IconvTest, str_to_wstr) {
  EXPECT_EQ(iconv.strToWstr("abc"), L"abc");
  EXPECT_EQ(iconv.strToWstr("中文"), L"中文");
}

TEST_F(IconvTest, wstr_to_str) {
  EXPECT_EQ(iconv.wstrToStr(L"abc"), "abc");
  EXPECT_EQ(iconv.wstrToStr(L"中文"), "中文");
}

TEST_F(IconvTest, utf8_to_wstr) {
  EXPECT_EQ(iconv.utf8ToWstr("abc"), L"abc");
  EXPECT_EQ(iconv.utf8ToWstr("\xE7\x83\xAB"), L"烫");
}

TEST_F(IconvTest, wstr_to_utf8) {
  EXPECT_EQ(iconv.wstrToUtf8(L"abc"), "abc");
  EXPECT_EQ(iconv.wstrToUtf8(L"烫"), "\xE7\x83\xAB");
}

TEST_F(IconvTest, utf8_to_str) {
  EXPECT_EQ(iconv.utf8ToStr("abc"), "abc");
  EXPECT_EQ(iconv.utf8ToStr("\xE7\x83\xAB"), "烫"); // GBK: "\xCC\xCC"
  EXPECT_EQ(iconv.utf8ToStr("\xE7\x83\xAB"), "\xCC\xCC");
}

TEST_F(IconvTest, str_to_utf8) {
  EXPECT_EQ(iconv.strToUtf8("abc"), "abc");
  EXPECT_EQ(iconv.strToUtf8("烫"), "\xE7\x83\xAB");
}

TEST_F(IconvTest, encode) {

}

TEST_F(IconvTest, decode) {

}

TEST_F(IconvTest, charCodeAt) {
  string str = "abc中文";
  EXPECT_EQ(iconv.charCodeAt(str, 0), 97);
  EXPECT_EQ(iconv.charCodeAt(str, 1), 98);
  EXPECT_EQ(iconv.charCodeAt(str, 2), 99);
  EXPECT_EQ(iconv.charCodeAt(str, 3), 20013);
  EXPECT_EQ(iconv.charCodeAt(str, 4), 25991);
}

TEST_F(IconvTest, utf8Length) {
  string str = "abc中文";
  EXPECT_EQ(iconv.utf8Length(iconv.strToUtf8(str)), 5);
  EXPECT_EQ(iconv.ansiLength(str), 5);
}