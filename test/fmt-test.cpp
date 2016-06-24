#include "stdafx.h"
#include "fmt-test.h"

TEST_F(FmtTest, fmt_format) {
  EXPECT_EQ(fmt::format("The answer is {:#x} {}", 42, "name"), "The answer is 0x2a name");

  fmt::MemoryWriter w;
  w.write("Look, a {} string", 'C');
  EXPECT_EQ(std::string(w.c_str()), "Look, a C string");
  

  EXPECT_EQ(fmt::format("{0} {1} {2}", 1, 1.23, false), "1 1.23 false");

  // fmt::bin, fmt::oct, fmt::hex, fmt::hexu
  fmt::MemoryWriter out;
  out << "The answer in hexadecimal is " << fmt::hex(42);
  EXPECT_EQ(out.str(), "The answer in hexadecimal is 2a");

  // 要格式化"%", 请使用fmt::sprintf
  EXPECT_EQ(fmt::format("%d", 13), "%d");
  EXPECT_EQ(fmt::format("%.2f", 1.23), "%.2f");
}

// 和format一样， 只是输出到stdout
TEST_F(FmtTest, fmt_print) {
  // 输出到stdout
  fmt::print("I'd rather be {1} than {0}. {1}\n", "right", "happy");
  EXPECT_EQ(fmt::format("{1} {2} {1}", "a", "b", "c"), "b c b");

  fmt::print("Hello, {name}! The answer is {number}. Goodbye, {name}.\n",
    fmt::arg("name", "World"), fmt::arg("number", 42));
}

// 类似于系统的printf
TEST_F(FmtTest, fmt_printf) {
  fmt::printf("Elapsed time: %.2f seconds", 1.23);
}

TEST_F(FmtTest, fmt_sprintf) {
  EXPECT_EQ(fmt::sprintf("%04d %.2f", 42, 1.456), "0042 1.46");
  EXPECT_EQ(fmt::sprintf("%s", "abc"), "abc");
  
  // 不支持宽字符到窄字符的转换
  // EXPECT_EQ(fmt::sprintf("%S", L"abc"), "abc");
}