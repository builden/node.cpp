#include "stdafx.h"
#include "iconv-test.h"

TEST_F(IconvTest, stow) {
  EXPECT_EQ(iconv.stow("abc"), L"abc");
  EXPECT_EQ(iconv.stow("中文"), L"中文");
}

TEST_F(IconvTest, wtos) {
  EXPECT_EQ(iconv.wtos(L"abc"), "abc");
  EXPECT_EQ(iconv.wtos(L"中文"), "中文");
}

TEST_F(IconvTest, u8tow) {
  EXPECT_EQ(iconv.u8tow("abc"), L"abc");
  EXPECT_EQ(iconv.u8tow("\xE7\x83\xAB"), L"烫");
}

TEST_F(IconvTest, wtou8) {
  EXPECT_EQ(iconv.wtou8(L"abc"), "abc");
  EXPECT_EQ(iconv.wtou8(L"烫"), "\xE7\x83\xAB");
}

TEST_F(IconvTest, u8tos) {
  EXPECT_EQ(iconv.u8tos("abc"), "abc");
  EXPECT_EQ(iconv.u8tos("\xE7\x83\xAB"), "烫"); // GBK: "\xCC\xCC"
  EXPECT_EQ(iconv.u8tos("\xE7\x83\xAB"), "\xCC\xCC");
}

TEST_F(IconvTest, stou8) {
  EXPECT_EQ(iconv.stou8("abc"), "abc");
  EXPECT_EQ(iconv.stou8("烫"), "\xE7\x83\xAB");
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

TEST_F(IconvTest, u8Length) {
  string str = "abc中文";
  EXPECT_EQ(iconv.u8Length(iconv.stou8(str)), 5);
  EXPECT_EQ(iconv.sLength(str), 5);
}

TEST_F(IconvTest, catchError) {
  string u8Str = u8"中b文a";
  string stou8Err = iconv.stou8(u8Str);
  cout << "stou8Err: " << stou8Err << endl;
  
#ifdef NDEBUG
  string ansiStr = "中文";
  string u8tosErr = iconv.u8tos(ansiStr);
  EXPECT_EQ(u8tosErr, "");

  wstring u8towErr = iconv.u8tow(ansiStr);
  EXPECT_EQ(u8towErr, L"");
#endif
}