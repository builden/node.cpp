#include "stdafx.h"
#include "fs-test.h"
#include <nodecpp/moment.h>

#include <fstream>

TEST_F(FsTest, open) {
  string noExitFile = "./abc.txt";
  fs.open(noExitFile, "r", [&](const Error& err, int fd) {
    EXPECT_TRUE(err);
    EXPECT_EQ(err.code(), -4058);
    EXPECT_EQ(err.name(), "ENOENT");
    EXPECT_EQ(err.str(), "no such file or directory");
    EXPECT_LE(fd, 0);
  });

  string exitFile = "C:\\Windows\\notepad.exe";
  fs.open(exitFile, "r", [&](const Error& err, int fd) {
    EXPECT_FALSE(err);
    EXPECT_GE(fd, 0);
  });

  run();
}

TEST_F(FsTest, readFile) {
  string file = path.dirname(process.execPath);
  // file = path.join(file, "../test/fs-test.cpp");

  // libuv 路径需要采用utf8编码
  // string file = "F:\\中文 中文\\我是中国人.txt";
  fs.readFile(file, [&](const Error& err, const Buffer& buf) {
    try {
      EXPECT_EQ(buf, fs.readFileSync(file));
      fmt::print("read file succ: {}\n", file);
    }
    catch (const Error& err) {
      EXPECT_EQ(err.str(), "illegal operation on a directory");
    }
  });

/*
  fs.readFile("C:\\Windows\\not.exe", [](const Error& err, const Buffer&buf) {
    EXPECT_TRUE(err);
    EXPECT_EQ("no such file or directory", err.str());
  });

  try {
    fs.readFileSync("K:/1");
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    EXPECT_EQ("no such file or directory", err.str());
  }*/

  run();
}

TEST_F(FsTest, writeFile) {
  string file = "D:/1.txt";

  /*fs.writeFileSync(file, Buffer("ad"));*/

/*
  fs.writeFile(file, Buffer("abcd中文a"), [](const Error& err) {
    EXPECT_FALSE(err);
  });*/

  string notexit = "K:/noexit/1.txt";
  fs.writeFile(notexit, Buffer(), [](const Error& err) {
    EXPECT_TRUE(err);
    EXPECT_EQ(err.str(), "no such file or directory");
  });

  try {
    fs.writeFileSync(notexit, Buffer());
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    EXPECT_EQ("no such file or directory", err.str());
  }
  
  run();
}

TEST_F(FsTest, stat) {
  string filePath = "C:\\windows\\notepad.exe";
  auto fileStat = fs.statSync(filePath);
  fmt::print("{}\n", fileStat.atime.format());
  EXPECT_TRUE(fileStat.isFile());
  EXPECT_FALSE(fileStat.isDirectory());
  EXPECT_FALSE(fileStat.isSymbolicLink());
  
  string dir1 = "C:\\";
  auto dirStat = fs.statSync(dir1);
  EXPECT_FALSE(dirStat.isFile());
  EXPECT_TRUE(dirStat.isDirectory());
  EXPECT_FALSE(dirStat.isSymbolicLink());

  try {
    string notexit = "C:\\1";
    fs.statSync(notexit);
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    EXPECT_EQ("no such file or directory", err.str());
  }
}

TEST_F(FsTest, exist) {
  EXPECT_TRUE(fs.existsSync("C://"));
  EXPECT_FALSE(fs.existsSync("C://noteexist"));
}

TEST_F(FsTest, rmdir) {
  try {
    fs.rmdirSync("C:\\windows");
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    // EXPECT_EQ("resource busy or locked", err.str());
  }
}

TEST_F(FsTest, unlink) {
  try {
    fs.unlinkSync("K://notexist");
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    EXPECT_EQ("no such file or directory", err.str());
  }
}

TEST_F(FsTest, mkdir) {
  try {
    fs.mkdirSync("D:/1/2/3/4/5");
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    // EXPECT_EQ("no such file or directory", err.str());
  }
}

TEST_F(FsTest, rename) {
  try {
    fs.renameSync("D:/1/2/3/4/5", "D://1/2/3/4/6");
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    // EXPECT_EQ("no such file or directory", err.str());
  }

  // 文件被占用
  // resource busy or locked
  // fs.renameSync("D:/1", "D:/2");
}

TEST_F(FsTest, access) {
  try {
    fs.accessSync("D:/notexist.txt");
    // will not execute at here
    EXPECT_FALSE(true);
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    // EXPECT_EQ(err.str(), "no such file or directory");
  }

  fs.access("D:/notexits.txt", [](const Error& err) {
    EXPECT_TRUE(err);
  });

  fs.access("C:\\windows\\notepad.exe", [](const Error& err) {
    EXPECT_FALSE(err);
  });

  run();
}