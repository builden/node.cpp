#include "dll-util.h"

namespace nodepp {

  string getDllPath(HMODULE hModule) {
    char path[MAX_PATH] = { 0 };
    GetModuleFileNameA(hModule, path, MAX_PATH);
    return path;
  }

}