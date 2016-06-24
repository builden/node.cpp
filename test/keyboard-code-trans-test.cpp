#include "stdafx.h"
#include "keyboard-code-trans-test.h"
#include <nodecpp/keyboard-code-trans.h>

TEST_F(KeyboardCodeTransTest, codes) {
  bool shifted = false;
  EXPECT_EQ(keyboardCodeFromStr("up", &shifted), VKEY_UP);
  EXPECT_EQ(keyboardCodeFromStr("cmd", &shifted), VKEY_COMMAND);
  EXPECT_EQ(keyboardCodeFromStr("ctrl", &shifted), VKEY_CONTROL);
  EXPECT_EQ(keyboardCodeFromStr("Shift", &shifted), VKEY_SHIFT);
  EXPECT_EQ(keyboardCodeFromStr("1", &shifted), VKEY_1);
  EXPECT_FALSE(shifted);
  EXPECT_EQ(keyboardCodeFromStr(")", &shifted), VKEY_0);
  EXPECT_TRUE(shifted);

  shifted = false;
  auto codes = keyboardCodesFromStr("Shift+Tab", &shifted);
  EXPECT_EQ(codes[0], VKEY_SHIFT);
  EXPECT_EQ(codes[1], VKEY_TAB);
  EXPECT_FALSE(shifted);
}