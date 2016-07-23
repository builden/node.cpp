#include "nodecpp-internals.h"
#include "nodecpp-def.h"
#include <uv.h>

namespace nodecpp {
  static void PrintErrorString(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
#ifdef _WIN32
    HANDLE stderr_handle = GetStdHandle(STD_ERROR_HANDLE);

    // Check if stderr is something other than a tty/console
    if (stderr_handle == INVALID_HANDLE_VALUE ||
      stderr_handle == nullptr ||
      uv_guess_handle(_fileno(stderr)) != UV_TTY) {
      vfprintf(stderr, format, ap);
      va_end(ap);
      return;
    }

    // Fill in any placeholders
    int n = _vscprintf(format, ap);
    std::vector<char> out(n + 1);
    vsprintf_s(out.data(), n, format, ap);

    // Get required wide buffer size
    n = MultiByteToWideChar(CP_UTF8, 0, out.data(), -1, nullptr, 0);

    std::vector<wchar_t> wbuf(n);
    MultiByteToWideChar(CP_UTF8, 0, out.data(), -1, wbuf.data(), n);
    WriteConsoleW(stderr_handle, wbuf.data(), n, nullptr, nullptr);
#else
    vfprintf(stderr, format, ap);
#endif
    va_end(ap);
  }

  static void OnFatalError(const char* location, const char* message) {
    if (location) {
      PrintErrorString("FATAL ERROR: %s %s\n", location, message);
    }
    else {
      PrintErrorString("FATAL ERROR: %s\n", message);
    }
    fflush(stderr);
    ABORT();
  }

  void FatalError(const char* location, const char* message) {
    OnFatalError(location, message);
    // to suppress compiler warning
    ABORT();
  }

}