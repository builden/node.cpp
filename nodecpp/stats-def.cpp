#include "stats-def.h"
#include "uv-win.h"

namespace nodecpp {

  bool Stats::isFile() const {
    return ((mode & S_IFMT) == S_IFREG);
  }

  bool Stats::isDirectory() const {
    return ((mode & S_IFMT) == S_IFDIR);
  }

  bool Stats::isCharacterDevice() const {
    return ((mode & S_IFMT) == S_IFCHR);
  }

  bool Stats::isSymbolicLink() const {
    return ((mode & S_IFMT) == S_IFLNK);
  }

}