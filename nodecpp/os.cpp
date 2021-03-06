#include "os.h"
#include "process.h"
#include <uv.h>
#include <windows.h>
#include "error.h"

namespace nodecpp {

  string Os::platform() {
    return process.platform;
  }

  string Os::tmpdir() {
    return process.getEnv("temp");
  }

  string Os::homedir() {
    char buf[MAX_PATH] = { 0 };

    size_t len = sizeof(buf);
    const int err = uv_os_homedir(buf, &len);
    if (err) throw Error(err);
    return buf;
  }

  string Os::arch() {
    return process.arch;
  }

  Os& os = Os::instance();

}
