// sub-process.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <nodecpp/nodecpp.h>
#include <nodecpp/fmt/format.h>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
  fmt::print(stdout, "stdout output 中文{}\n", 1);
  fmt::print(stderr, "stderr output 中文{}\n", 2);

  for (int i = 0; i < argc; ++i) {
    wcout << "argv[" << i << "]: " << argv[i] << endl;
  }

  cout << endl;

  for (int i = 0; i < 10; ++i) {
    cout << "at " << i << endl;
    Sleep(400);
  }
	return 2;
}

