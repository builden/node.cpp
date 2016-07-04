#include "stdafx.h"
#include "winreg-test.h"
#include <nodecpp/winreg.h>

TEST_F(WinregTest, get) {
  Winreg reg(Winreg::HKLM, "SOFTWARE\\Wow6432Node\\Tencent\\QQ2009");
  string value;
  EXPECT_TRUE(reg.get("Install", value));
  cout << "Install: " << value << endl;
  EXPECT_FALSE(reg.get("InstallNot", value));

  uint32_t nShortcutDesktop = 0;
  EXPECT_TRUE(reg.get("nShortcutDesktop", nShortcutDesktop));
  EXPECT_EQ(nShortcutDesktop, 1);

  EXPECT_TRUE(reg.set("nShortcutDesktop", 3u));
  reg.get("nShortcutDesktop", nShortcutDesktop);
  EXPECT_EQ(nShortcutDesktop, 3);
  EXPECT_TRUE(reg.set("nShortcutDesktop", 1u));

//   EXPECT_TRUE(reg.set("testString", "abc中文"));
//   string testString;
//   EXPECT_TRUE(reg.get("testString", testString));
//   EXPECT_EQ(testString, "abc中文");
}
