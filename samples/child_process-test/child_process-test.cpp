// child_process-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <nodecpp/nodecpp.h>
using namespace nodecpp;
#pragma comment(lib, "legacy_stdio_definitions.lib")

int _tmain(int argc, _TCHAR* argv[])
{
  // auto child = child_process.spawn("notepad.exe", svec_t{"abc"});
  auto child = child_process.spawn("notepad.exe");
  run();
	return 0;
}

