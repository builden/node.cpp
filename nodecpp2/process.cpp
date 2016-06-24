#include "process.h"
#include <windows.h>
#include <stdlib.h>
#include "timers.h"
#include "iconv.h"

namespace nodepp {

  Process::Process() {
    char cur_path[MAX_PATH] = { 0 };
    ::GetCurrentDirectoryA(MAX_PATH, cur_path);
    cwd_ = cur_path;

    char exec_path[MAX_PATH] = { 0 };
    ::GetModuleFileNameA(NULL, exec_path, MAX_PATH);
    execPath = exec_path;

    pid = ::GetCurrentProcessId();

    initArgv();
    initEnv();
  }

  string Process::cwd() {
    return cwd_;
  }

  Process &process = Process::instance();

  void Process::nextTick(NextTickCb_t cb) {
    setTimeout(cb, 0);
  }

  void Process::initArgv() {
    LPWSTR *szArglist = nullptr;
    int nArgs = 0;
    szArglist = CommandLineToArgvW(GetCommandLine(), &nArgs);
    if (nullptr == szArglist) {
      throw "CommandLineToArgvW failed";
      return;
    }
    else {
      for (int i = 0; i < nArgs; i++) {
        argv.emplace_back(iconv.wstrToStr(szArglist[i]));
        // printf("%d: %ws\n", i, szArglist[i]);
      }
    }
    LocalFree(szArglist);
  }

  void Process::initEnv() {
    WCHAR* environment = GetEnvironmentStringsW();
    if (environment == nullptr)
      return;  // This should not happen.
    Json::object j{};
    WCHAR* p = environment;
    while (*p) {
      WCHAR *s;
      if (*p == L'=') {
        // If the key starts with '=' it is a hidden environment variable.
        p += wcslen(p) + 1;
        continue;
      }
      else {
        s = wcschr(p, L'=');
      }
      if (!s) {
        s = p + wcslen(p);
      }
      const uint16_t* two_byte_buffer = reinterpret_cast<const uint16_t*>(p);
      const size_t two_byte_buffer_len = s - p;
      
      std::wstring key(p, two_byte_buffer_len);
      std::wstring value = s + 1;
      j[iconv.wstrToStr(key)] = iconv.wstrToStr(value);
      
      p = s + wcslen(s) + 1;
    }

    FreeEnvironmentStringsW(environment);

    env = j;
  }

  string Process::getEnv(const string& envVar) {
    string rst;
    char* libvar = nullptr;
    size_t requiredSize = 0;

    getenv_s(&requiredSize, NULL, 0, envVar.c_str());
    if (requiredSize == 0) {
      printf("LIB doesn't exist!\n");
      return rst;
    }

    libvar = (char*)malloc(requiredSize * sizeof(char));
    if (!libvar) {
      printf("Failed to allocate memory!\n");
      return rst;
    }

    // Get the value of the LIB environment variable.
    getenv_s(&requiredSize, libvar, requiredSize, envVar.c_str());

    rst = libvar;
    free(libvar);
    return rst;
  }

}