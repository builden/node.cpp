#ifndef __DLL_UTIL_H__
#define __DLL_UTIL_H__

#include "nodecpp-def.h"
#include <windows.h>

namespace nodecpp {
  using std::string;

  // HMODULE from dllmain function param
  string getDllPath(HMODULE hModule);
}

#endif // !__DLL_UTIL_H__
