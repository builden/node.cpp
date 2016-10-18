#include "stdafx.h"
#include "process-test.h"
#include <windows.h>

TEST_F(ProcessTest, execPath) {
  char exec_path[MAX_PATH] = { 0 };
  ::GetModuleFileNameA(NULL, exec_path, MAX_PATH);
  EXPECT_EQ(process.execPath, exec_path);
}

TEST_F(ProcessTest, cwd) {
  char cwd_path[MAX_PATH] = { 0 };
  ::GetCurrentDirectoryA(MAX_PATH, cwd_path);
  EXPECT_EQ(process.cwd(), cwd_path);
}

TEST_F(ProcessTest, pid) {
  DWORD pid = ::GetCurrentProcessId();
  EXPECT_EQ(pid, process.pid);
}

TEST_F(ProcessTest, nextTick) {
  process.nextTick([]() {
    cout << "nextTick" << endl;
  });
  run();
}

TEST_F(ProcessTest, argv) {
  EXPECT_TRUE(s.endsWith(process.argv[0], "\\test.exe"));
  EXPECT_GE(process.argv.size(), 1u);
  for (auto& arg : process.argv) {
    cout << "argv: " << arg << endl;
  }
}

TEST_F(ProcessTest, env) {
  json j = process.env;
  for (auto it = j.begin(); it != j.end(); ++it) {
    cout << it.key() << ":" << it.value() << endl;
  }

  EXPECT_EQ(process.env["NOTEXIST"].is_null(), true);
  EXPECT_EQ(s.toLower(process.env["windir"].get<string>()), "c:\\windows");
}

TEST_F(ProcessTest, versions) {
  cout << "version: " << process.version << endl;

  cout << "versions: " << process.versions.dump() << endl;
}