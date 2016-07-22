#include "util.h"
#include <uv.h>

namespace nodecpp {

  void Abort() {
    DumpBacktrace(stderr);
    fflush(stderr);
    ABORT_NO_BACKTRACE();
  }

  void Assert(const char* const (*args)[4]) {
    auto filename = (*args)[0];
    auto linenum = (*args)[1];
    auto message = (*args)[2];
    auto function = (*args)[3];

    char exepath[256];
    size_t exepath_size = sizeof(exepath);
    if (uv_exepath(exepath, &exepath_size))
      snprintf(exepath, sizeof(exepath), "node");

    char pid[12] = { 0 };

    fprintf(stderr, "%s%s: %s:%s:%s%s Assertion `%s' failed.\n",
      exepath, pid, filename, linenum,
      function, *function ? ":" : "", message);
    fflush(stderr);

    Abort();
  }

  void DumpBacktrace(FILE* /*fp*/) {

  }

}