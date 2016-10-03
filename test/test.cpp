// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment(lib, "dbghelp.lib")

#ifdef _DEBUG
#ifdef _DLL
#pragma comment(lib, "gtest-d.lib")
#else  // _DLL
#pragma comment(lib, "gtest-sd.lib")
#endif // _DLL
#else  // _DEBUG
#ifdef _DLL
#pragma comment(lib, "gtest.lib")
#else  // _DLL
#pragma comment(lib, "gtest-s.lib")
#endif // _DLL
#endif // _DEBUG

int _tmain(int argc, _TCHAR* argv[])
{
  ::testing::GTEST_FLAG(filter) = "*";
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
