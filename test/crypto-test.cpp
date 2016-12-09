#include "stdafx.h"
#include "crypto-test.h"

TEST_F(CryptoTest, md5) {
  EXPECT_EQ(crypto.createHash("md5").update(Buffer("abc")).digest().toString("hex"), "900150983cd24fb0d6963f7d28e17f72");
  EXPECT_EQ(crypto.md5(Buffer("abc")).toString("hex"), "900150983cd24fb0d6963f7d28e17f72");
  EXPECT_EQ(crypto.md5(Buffer("ºº×Ö")).toString("hex"), "8a97ee1fcddc24870fb66b4b58c41214");

  EXPECT_EQ(crypto.md5(Buffer()).toString("hex"), "d41d8cd98f00b204e9800998ecf8427e");
}