#include "stdafx.h"
#include "crypto-test.h"

TEST_F(CryptoTest, md5) {
  EXPECT_EQ(crypto.createHash("md5").update(Buffer("abc")).digest().toString("hex"), "900150983cd24fb0d6963f7d28e17f72");
  EXPECT_EQ(crypto.md5(Buffer("abc")).toString("hex"), "900150983cd24fb0d6963f7d28e17f72");
  EXPECT_EQ(crypto.md5(Buffer("ºº×Ö")).toString("hex"), "8a97ee1fcddc24870fb66b4b58c41214");

  EXPECT_EQ(crypto.md5(Buffer()).toString("hex"), "d41d8cd98f00b204e9800998ecf8427e");
}

TEST_F(CryptoTest, sha1) {
  EXPECT_EQ(crypto.createHash("sha1").update(Buffer("abc")).digest().toString("hex"), "a9993e364706816aba3e25717850c26c9cd0d89d");
  EXPECT_EQ(crypto.sha1(Buffer("abc")).toString("hex"), "a9993e364706816aba3e25717850c26c9cd0d89d");
  EXPECT_EQ(crypto.sha1(Buffer("ºº×Ö")).toString("hex"), "c06ce2d7fdeda6ff7629156bee66bf4d818ab397");

  EXPECT_EQ(crypto.sha1(Buffer()).toString("hex"), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST_F(CryptoTest, sha256) {
  EXPECT_EQ(crypto.createHash("sha256").update(Buffer("abc")).digest().toString("hex"), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  EXPECT_EQ(crypto.sha256(Buffer("abc")).toString("hex"), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  EXPECT_EQ(crypto.sha256(Buffer("ºº×Ö")).toString("hex"), "00e5d5601ce310d84ec2875e28c826f7f4c0a473be7fd53cf50e47f1542db4ea");

  EXPECT_EQ(crypto.sha256(Buffer()).toString("hex"), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}