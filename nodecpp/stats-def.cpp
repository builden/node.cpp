#include "stats-def.h"
#include "uv-win.h"

namespace nodecpp {

  bool Stats::isFile() {
    return ((mode & S_IFMT) == S_IFREG);
  }

  bool Stats::isDirectory() {
    return ((mode & S_IFMT) == S_IFDIR);
  }

  bool Stats::isCharacterDevice() {
    return ((mode & S_IFMT) == S_IFCHR);
  }

  bool Stats::isSymbolicLink() {
    return ((mode & S_IFMT) == S_IFLNK);
  }

}