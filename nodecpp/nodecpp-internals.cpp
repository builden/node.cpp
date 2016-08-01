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

  const char *signo_string(int signo) {
#define SIGNO_CASE(e)  case e: return #e;
    switch (signo) {
#ifdef SIGHUP
      SIGNO_CASE(SIGHUP);
#endif

#ifdef SIGINT
      SIGNO_CASE(SIGINT);
#endif

#ifdef SIGQUIT
      SIGNO_CASE(SIGQUIT);
#endif

#ifdef SIGILL
      SIGNO_CASE(SIGILL);
#endif

#ifdef SIGTRAP
      SIGNO_CASE(SIGTRAP);
#endif

#ifdef SIGABRT
      SIGNO_CASE(SIGABRT);
#endif

#ifdef SIGIOT
# if SIGABRT != SIGIOT
      SIGNO_CASE(SIGIOT);
# endif
#endif

#ifdef SIGBUS
      SIGNO_CASE(SIGBUS);
#endif

#ifdef SIGFPE
      SIGNO_CASE(SIGFPE);
#endif

#ifdef SIGKILL
      SIGNO_CASE(SIGKILL);
#endif

#ifdef SIGUSR1
      SIGNO_CASE(SIGUSR1);
#endif

#ifdef SIGSEGV
      SIGNO_CASE(SIGSEGV);
#endif

#ifdef SIGUSR2
      SIGNO_CASE(SIGUSR2);
#endif

#ifdef SIGPIPE
      SIGNO_CASE(SIGPIPE);
#endif

#ifdef SIGALRM
      SIGNO_CASE(SIGALRM);
#endif

      SIGNO_CASE(SIGTERM);

#ifdef SIGCHLD
      SIGNO_CASE(SIGCHLD);
#endif

#ifdef SIGSTKFLT
      SIGNO_CASE(SIGSTKFLT);
#endif


#ifdef SIGCONT
      SIGNO_CASE(SIGCONT);
#endif

#ifdef SIGSTOP
      SIGNO_CASE(SIGSTOP);
#endif

#ifdef SIGTSTP
      SIGNO_CASE(SIGTSTP);
#endif

#ifdef SIGBREAK
      SIGNO_CASE(SIGBREAK);
#endif

#ifdef SIGTTIN
      SIGNO_CASE(SIGTTIN);
#endif

#ifdef SIGTTOU
      SIGNO_CASE(SIGTTOU);
#endif

#ifdef SIGURG
      SIGNO_CASE(SIGURG);
#endif

#ifdef SIGXCPU
      SIGNO_CASE(SIGXCPU);
#endif

#ifdef SIGXFSZ
      SIGNO_CASE(SIGXFSZ);
#endif

#ifdef SIGVTALRM
      SIGNO_CASE(SIGVTALRM);
#endif

#ifdef SIGPROF
      SIGNO_CASE(SIGPROF);
#endif

#ifdef SIGWINCH
      SIGNO_CASE(SIGWINCH);
#endif

#ifdef SIGIO
      SIGNO_CASE(SIGIO);
#endif

#ifdef SIGPOLL
# if SIGPOLL != SIGIO
      SIGNO_CASE(SIGPOLL);
# endif
#endif

#ifdef SIGLOST
# if SIGLOST != SIGABRT
      SIGNO_CASE(SIGLOST);
# endif
#endif

#ifdef SIGPWR
# if SIGPWR != SIGLOST
      SIGNO_CASE(SIGPWR);
# endif
#endif

#ifdef SIGINFO
# if !defined(SIGPWR) || SIGINFO != SIGPWR
      SIGNO_CASE(SIGINFO);
# endif
#endif

#ifdef SIGSYS
      SIGNO_CASE(SIGSYS);
#endif

    default: return "";
    }
  }
}