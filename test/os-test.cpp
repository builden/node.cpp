#include "stdafx.h"
#include "os-test.h"

TEST_F(OsTest, tmpdir) {
  EXPECT_EQ(os.platform(), "win32");
  cout << os.tmpdir() << endl;
  cout << os.homedir() << endl;
}