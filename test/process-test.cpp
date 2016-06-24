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
  Json::object j = process.env.object_items();
  for (auto obj : j) {
    cout << obj.first  << ":" << obj.second.string_value() << endl;
  }

  EXPECT_EQ(process.env["NOTEXIST"].is_null(), true);
  EXPECT_EQ(process.env["NOTEXIST"].string_value(), "");
  EXPECT_EQ(process.env["windir"].string_value(), "C:\\windows");
}