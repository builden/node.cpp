#include "stdafx.h"
#include "path-test.h"

TEST_F(PathTest, basename) {
  auto test_path = "C:\\Windows\\notepad.exe";
  EXPECT_EQ(path.dirname(test_path), "C:\\Windows");
  auto extname = path.extname(test_path);
  EXPECT_EQ(extname, ".exe");

  EXPECT_EQ(path.basename(test_path), "notepad.exe");
  EXPECT_EQ(path.basename(test_path, extname), "notepad");
}

TEST_F(PathTest, isAbsolute) {
  EXPECT_TRUE(path.isAbsolute("//server"));
  EXPECT_TRUE(path.isAbsolute("C:/foo/.."));
  EXPECT_FALSE(path.isAbsolute("bar\\baz"));
  EXPECT_FALSE(path.isAbsolute("."));
}

TEST_F(PathTest, normalize) {
  EXPECT_EQ(path.normalize("/foo/bar//baz/asdf/quux/.."), "\\foo\\bar\\baz\\asdf");
  EXPECT_EQ(path.normalize("C:\\a\\..\\b\\.\\c"), "C:\\b\\c");
}

TEST_F(PathTest, join) {
  EXPECT_EQ(path.join("/foo", "baz\\asdf", "quux", ".."), "\\foo\\baz\\asdf");
  EXPECT_EQ(path.join("C:\\foo", "baz\\asdf", "..", "abc"), "C:\\foo\\baz\\abc");
}

TEST_F(PathTest, resolve) {
  EXPECT_TRUE(s.endsWith(path.resolve("/foo/bar", "./baz"), "\\foo\\bar\\baz"));

  EXPECT_TRUE(s.endsWith(path.resolve("D:\\a", "b\\c", "..\\d"), "D:\\a\\b\\d"));

  cout << process.cwd() << endl;
  cout << path.resolve("a") << endl;

  EXPECT_EQ(path.resolve("a"), path.join(process.cwd(), "a"));

  EXPECT_EQ(path.resolve("%windir%"), "C:\\windows");
}