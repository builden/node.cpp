#include "stdafx.h"
#include "core-test.h"

TEST_F(CoreTest, encodeURI) {
  EXPECT_EQ(encodeURI("http://www.w3school.com.cn/My first/"), "http://www.w3school.com.cn/My%20first/");

  EXPECT_EQ(encodeURI(";,/?:@&=+$#"), ";,/?:@&=+$#");

  EXPECT_EQ(encodeURI("http://www.qq.com?name=中文"), "http://www.qq.com?name=%E4%B8%AD%E6%96%87");
}

TEST_F(CoreTest, decodeURI) {
  EXPECT_EQ(decodeURI("http://www.w3school.com.cn/My%20first/"), "http://www.w3school.com.cn/My first/");
  EXPECT_EQ(decodeURI(";,/?:@&=+$#"), ";,/?:@&=+$#");
  EXPECT_EQ(decodeURI("http://www.qq.com?name=%E4%B8%AD%E6%96%87"), "http://www.qq.com?name=中文");
}

TEST_F(CoreTest, encodeURIComponent) {
  EXPECT_EQ(encodeURIComponent("http://www.w3school.com.cn/My first/"), "http%3A%2F%2Fwww.w3school.com.cn%2FMy%20first%2F");

  EXPECT_EQ(encodeURIComponent(";,/?:@&=+$#"), "%3B%2C%2F%3F%3A%40%26%3D%2B%24%23");

  EXPECT_EQ(encodeURIComponent("http://www.qq.com?name=中文"), "http%3A%2F%2Fwww.qq.com%3Fname%3D%E4%B8%AD%E6%96%87");
}

TEST_F(CoreTest, decodeURIComponent) {
  EXPECT_EQ(decodeURIComponent("http%3A%2F%2Fwww.w3school.com.cn%2FMy%20first%2F"), "http://www.w3school.com.cn/My first/");

  EXPECT_EQ(decodeURIComponent("%3B%2C%2F%3F%3A%40%26%3D%2B%24%23"), ";,/?:@&=+$#");

  EXPECT_EQ(decodeURIComponent("http%3A%2F%2Fwww.qq.com%3Fname%3D%E4%B8%AD%E6%96%87"), "http://www.qq.com?name=中文");
}