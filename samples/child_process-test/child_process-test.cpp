// child_process-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <nodecpp/nodecpp.h>
using namespace nodecpp;
#pragma comment(lib, "legacy_stdio_definitions.lib")

int _tmain(int argc, _TCHAR* argv[])
{
  auto child = child_process.spawn();
  run();
	return 0;
}

