#include "stdafx.h"
#include "file-url-test.h"
#include <nodecpp/file-url.h>

TEST_F(FileUrlTest, fileUrl) {
  EXPECT_EQ(fileUrl("D:\\a\\b\\c"), "file:///D:/a/b/c");

  std::string expectUrl = "file:///";
  expectUrl += s.replaceAll(path.resolve("abc"), "\\", "/");
  EXPECT_EQ(fileUrl("abc"), expectUrl);

  // utf8 ±àÂë
  EXPECT_EQ(fileUrl("D:\\ÖÐÎÄ"), "file:///D:/%E4%B8%AD%E6%96%87");
}