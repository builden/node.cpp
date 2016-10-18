#include "process.h"
#include "nodecpp-version.h"
#include <uv.h>
#include <zlib.h>

#include <windows.h>
#include <stdlib.h>
#include "timers.h"
#include "iconv.h"

namespace nodecpp {

  class Process::impl {
  public:
    void initArgv(svec_t& argv);
    void initEnv(json& env);

    string cwd_;
  };

  Process::Process() : pimpl(new Process::impl) {
    char cur_path[MAX_PATH] = { 0 };
    ::GetCurrentDirectoryA(MAX_PATH, cur_path);
    pimpl->cwd_ = cur_path;

    char exec_path[MAX_PATH] = { 0 };
    ::GetModuleFileNameA(NULL, exec_path, MAX_PATH);
    execPath = exec_path;

    pid = ::GetCurrentProcessId();

    // for Math.random and _.shuffle
    std::srand((uint32_t)time(0));

    pimpl->initArgv(argv);
    pimpl->initEnv(env);

#ifdef _WIN64
    arch = "x64";
#else
    arch = "ia32";
#endif

    version = NODECPP_VERSION;
    versions = {
      { "nodecpp", NODECPP_VERSION + 1 },
      { "uv", uv_version_string() },
      { "zlib", ZLIB_VERSION }
    };
  }

  string Process::cwd() {
    return pimpl->cwd_;
  }

  Process &process = Process::instance();

  void Process::nextTick(NextTickCb_t cb) {
    setTimeout(cb, 0);
  }

  void Process::impl::initArgv(svec_t& argv) {
    LPWSTR *szArglist = nullptr;
    int nArgs = 0;
    szArglist = CommandLineToArgvW(GetCommandLine(), &nArgs);
    if (nullptr == szArglist) {
      throw "CommandLineToArgvW failed";
      return;
    }
    else {
      for (int i = 0; i < nArgs; i++) {
        argv.emplace_back(iconv.wtos(szArglist[i]));
        // printf("%d: %ws\n", i, szArglist[i]);
      }
    }
    LocalFree(szArglist);
  }

  void Process::impl::initEnv(json& env) {
    WCHAR* environment = GetEnvironmentStringsW();
    if (environment == nullptr)
      return;  // This should not happen.
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
      // const uint16_t* two_byte_buffer = reinterpret_cast<const uint16_t*>(p);
      const size_t two_byte_buffer_len = s - p;
      
      std::wstring key(p, two_byte_buffer_len);
      std::wstring value = s + 1;
      env[iconv.wtos(key)] = iconv.wtos(value);
      
      p = s + wcslen(s) + 1;
    }

    FreeEnvironmentStringsW(environment);
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