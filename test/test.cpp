// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "gtest.lib")
#pragma comment(lib, "nodecpp.lib")

int _tmain(int argc, _TCHAR* argv[])
{
  // ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

