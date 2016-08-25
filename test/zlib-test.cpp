#include "stdafx.h"
#include "zlib-test.h"

TEST_F(ZlibTest, deflate) {
  EXPECT_EQ(zlib.deflateSync(Buffer("a")).toString("hex"), "789c4b040000620062");
  EXPECT_EQ(zlib.deflateSync(Buffer("b")).toString("hex"), "789c4b020000630063");
}

TEST_F(ZlibTest, inflate) {
  EXPECT_EQ(zlib.inflateSync(Buffer("789c4b040000620062", "hex")).toString(), "a");
}

TEST_F(ZlibTest, deflateRaw) {
  EXPECT_EQ(zlib.deflateRawSync(Buffer("a")).toString("hex"), "4b0400");
}

TEST_F(ZlibTest, inflateRaw) {
  EXPECT_EQ(zlib.inflateRawSync(Buffer("4b0400", "hex")).toString(), "a");
}

TEST_F(ZlibTest, Gzip) {
  EXPECT_EQ(zlib.gzipSync(Buffer("a")).toString("hex"), "1f8b080000000000000b4b040043beb7e801000000");
}

TEST_F(ZlibTest, Gunzip) {
  EXPECT_EQ(zlib.gunzipSync(Buffer("1f8b080000000000000b4b040043beb7e801000000", "hex")).toString(), "a");
}

TEST_F(ZlibTest, unzip) {
  EXPECT_EQ(zlib.unzipSync(Buffer("789c4b040000620062", "hex")).toString(), "a");
  EXPECT_EQ(zlib.unzipSync(Buffer("1f8b080000000000000b4b040043beb7e801000000", "hex")).toString(), "a");
}

TEST_F(ZlibTest, unzipFile) {
  zlib.unzipFile("D:/notexit.zip", "D:/zip", [](const Error& err) {
    EXPECT_TRUE(err);
    EXPECT_EQ(err.str(), string("unzOpen64 D:/notexit.zip failed"));
  });

/*
  zlib.unzipFile("D:/dd.zip", "D:/zip", [](const Error& err) {
    EXPECT_FALSE(err);
    cout << err.str() << endl;
  });*/
  // zlib.unzipFileSync("D:/dd.zip", "D:/zip");

  run();
}