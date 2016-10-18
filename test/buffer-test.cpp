#include "stdafx.h"
#include "buffer-test.h"

TEST_F(BufferTest, alloc) {
  Buffer buf1 = Buffer::alloc(5);
  EXPECT_EQ(buf1.format(), "<Buffer 00 00 00 00 00>");
  
  Buffer buf2 = Buffer::alloc(5, 0x01);
  EXPECT_EQ(buf2.format(), "<Buffer 01 01 01 01 01>");
}

TEST_F(BufferTest, format) {
  Buffer buf1(0);
  EXPECT_EQ(buf1.format(), "<Buffer >");

  Buffer buf2(2);
  EXPECT_EQ(buf2.format(), "<Buffer 00 00>");

  Buffer buf3(5);
  EXPECT_EQ(buf3.format(5), "<Buffer 00 00 00 00 00>");
  EXPECT_EQ(buf3.format(4), "<Buffer 00 00 00 00 ... >");
}

TEST_F(BufferTest, constructor) {
  Buffer buf1(std::string("abcdef"));
  EXPECT_EQ(buf1.toString(), "abcdef");
  EXPECT_EQ(buf1.length(), 6);

  Buffer buf2(std::string("中文abcd"));
  EXPECT_EQ(buf2.toString(), "中文abcd");
  EXPECT_EQ(buf2.length(), iconv.stou8("中文abcd").length());

  Buffer buf3(std::wstring(L"abcdef"));
  EXPECT_EQ(buf3.length(), 6);

  Buffer buf4(std::wstring(L"中文abcd"));
  EXPECT_EQ(buf4.length(), string(u8"中文abcd").length());

  Buffer buf5("abcdef", 4);
  EXPECT_EQ(buf5.toString(), "abcd");
  EXPECT_EQ(buf5.length(), 4);

  Buffer buf6("ab中文", 4);
  EXPECT_EQ(buf6.format(), "<Buffer 61 62 d6 d0>");
  EXPECT_EQ(buf6.length(), 4);

  Buffer buf7(buf6);
  buf7[0] = 'b';
  EXPECT_EQ(buf7.format(), "<Buffer 62 62 d6 d0>");
  EXPECT_EQ(buf6.format(), "<Buffer 61 62 d6 d0>");

  Buffer buf8("YWJjZGVm", "base64");
  EXPECT_EQ(buf8.format(), "<Buffer 61 62 63 64 65 66>");

  Buffer buf9("YWJj5Lit5paHYWJj", "base64");
  EXPECT_EQ(buf9.format(), "<Buffer 61 62 63 e4 b8 ad e6 96 87 61 62 63>");

  EXPECT_EQ(Buffer("616263646566", "hex").toString(), "abcdef");
}

TEST_F(BufferTest, equals) {
  Buffer buf1("abc");
  Buffer buf2("abc");
  Buffer buf3("cbd");
  EXPECT_TRUE(buf1.equals(buf2));
  EXPECT_FALSE(buf2.equals(buf3));
}

TEST_F(BufferTest, write_read) {
  Buffer buf1(4);
  uint32_t offset = buf1.writeInt32LE(0xff345678);
  EXPECT_EQ(offset, 4);
  EXPECT_EQ(buf1.format(), "<Buffer 78 56 34 ff>");
  EXPECT_EQ(buf1.readInt8(3), -1);
  EXPECT_EQ(buf1.readUInt8(3), 255);
  EXPECT_EQ(buf1.readInt32LE(), -13347208);
  EXPECT_EQ(buf1.readInt32LE(), 0xff345678);

  Buffer buf2(4);
  offset = buf2.writeInt32BE(0xff345678);
  EXPECT_EQ(buf2.format(), "<Buffer ff 34 56 78>");
  EXPECT_EQ(buf2.readInt32BE(), -13347208);
  EXPECT_EQ(buf2.readInt32BE(), 0xff345678);

  // 超出范围，写失败
  Buffer buf3(3);
  offset = buf3.writeInt32BE(0xff345678);
  EXPECT_EQ(offset, 0);
  EXPECT_EQ(buf3.format(), "<Buffer 00 00 00>");

  Buffer buf4(5);
  offset = buf4.writeUInt32LE(0xff345678, 1);
  EXPECT_EQ(offset, 5);
  EXPECT_EQ(buf4.format(), "<Buffer 00 78 56 34 ff>");
  EXPECT_EQ(buf4.readUInt32LE(1), 4281620088);

  Buffer buf5(6);
  offset = buf5.writeUInt32BE(0xff345678, 2);
  EXPECT_EQ(offset, 6);
  EXPECT_EQ(buf5.format(), "<Buffer 00 00 ff 34 56 78>");
  EXPECT_EQ(buf5.readUInt32BE(2), 4281620088);
}

TEST_F(BufferTest, from) {

}

TEST_F(BufferTest, toString) {
  Buffer buf1("abcdef");
  EXPECT_EQ(buf1.toString("ansi", 1), "bcdef");
  EXPECT_EQ(buf1.toString("ansi", 1, 2), "b");
  EXPECT_EQ(buf1.toString("base64"), "YWJjZGVm");
  EXPECT_EQ(buf1.toString("hex"), "616263646566");

  Buffer buf2("abc中文abc");
  EXPECT_EQ(buf2.toString("ansi", 2), "c中文abc");
  EXPECT_EQ(buf2.toString("ansi", 3, 9), "中文");
  EXPECT_EQ(buf2.toString("utf8", 3, 9), "\xE4\xB8\xAD\xE6\x96\x87");
  EXPECT_EQ(buf2.toWstring(3, 9), L"中文");
  EXPECT_EQ(buf2.toString("base64"), "YWJj5Lit5paHYWJj");
  EXPECT_EQ(buf2.toString("hex"), "616263e4b8ade69687616263");
}

TEST_F(BufferTest, slice) {
  Buffer buf1("abcdef");
  Buffer buf2 = buf1.slice(1, -2);
  EXPECT_EQ(buf1.toString(), "abcdef");
  EXPECT_EQ(buf2.toString(), "bcd");
}

TEST_F(BufferTest, concat) {
  Buffer buf1("abc");
  Buffer buf2("cbd");
  Buffer buf3("efg");
  vector<Buffer> bufs = { buf1, buf2, buf3 };
  EXPECT_EQ(Buffer("ab"), Buffer::concat(bufs, 2));
  EXPECT_EQ(Buffer("abccbdefg"), Buffer::concat(bufs, 9));
  EXPECT_EQ(Buffer::concat(bufs, 10).format(), "<Buffer 61 62 63 63 62 64 65 66 67 00>");
}