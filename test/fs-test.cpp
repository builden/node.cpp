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

  fs.exists("C://noteexist", [](bool isExist) {
    EXPECT_FALSE(isExist);
  });
  run();
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

TEST_F(FsTest, readJson) {
  string file = "fs-test.json";
  fs.writeFileSync(file, Buffer(R"({ "a": "b" })"));
  EXPECT_TRUE(fs.existsSync(file));
  json cfg = fs.readJsonSync(file);
  EXPECT_EQ(cfg["a"].get<string>(), "b");
  fs.unlinkSync(file);
  EXPECT_FALSE(fs.existsSync(file));
}

TEST_F(FsTest, mkdirs) {
/*
  try {
    fs.mkdirsSync("D:/1/2/3/4");
  }
  catch (const Error&) {
    // 不会到这里来
    EXPECT_FALSE(true);
  }

  fs.mkdirs("D:/4/3/2/1", [](const Error& err) {
    EXPECT_FALSE(err);
  });
  run();*/
}

TEST_F(FsTest, readdir) {
/*
  try {
    svec_t dirs = fs.readdirSync("D:/中 文 a");
    for (auto& dir : dirs) {
      cout << "readdirSync: " << dir << endl;
    }
  }
  catch (const Error&) {
    EXPECT_TRUE(false);
  }

  fs.readdir("D:/中 文 a", [](const Error& err, const svec_t& dirs) {
    EXPECT_FALSE(err);
    for (auto& dir : dirs) {
      cout << "readdir: " << dir << endl;
    }
  });*/

  fs.readdir("D:/notexit", [](const Error& err, const svec_t& dirs) {
    EXPECT_TRUE(err);
  });

  run();
}

TEST_F(FsTest, remove) {
/*
  try {
    fs.removeSync("D:/1");
  }
  catch (const Error& err) {
    EXPECT_TRUE(err);
    if (err) cout << err.str() << endl;
  }*/

/*
  fs.remove("D:/1", [](const Error& err) {
    EXPECT_FALSE(err);
  });*/
  
  fs.remove("D:/notexist", [](const Error& err) {
    EXPECT_FALSE(err);
  });
  run();
}

TEST_F(FsTest, chinesePath) {
  string testPath = "D:\\中文 测试目录";
  fs.mkdirsSync(testPath);
  EXPECT_TRUE(fs.existsSync(testPath));
  string filePath = path.join(testPath, "中文2 中文.txt");
  fs.writeFileSync(filePath, Buffer("中文内容"));
  
  EXPECT_TRUE(fs.existsSync(filePath));

  Buffer buf = fs.readFileSync(filePath);
  EXPECT_EQ(buf.toU8string(), u8"中文内容");

  fs.unlinkSync(filePath);
  EXPECT_FALSE(fs.existsSync(filePath));
  EXPECT_TRUE(fs.existsSync(testPath));

  fs.removeSync(testPath);
  EXPECT_FALSE(fs.existsSync(testPath));
}

TEST_F(FsTest, walkSync) {
  try {
    svec_t filelist = fs.walkSync("D:\\1");
  }
  catch (std::exception&) {

  }
}