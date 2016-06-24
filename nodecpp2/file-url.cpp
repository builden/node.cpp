#include "file-url.h"
#include "core.h"
#include "underscore.string.h"
#include "path.h"

namespace nodepp {

  string fileUrl(const string& str, bool isResolve /*= true*/) {
    string pathName = str;
    if (isResolve) {
      pathName = path.resolve(str);
    }

    pathName = s.replaceAll(pathName, "\\", "/");

    // Windows drive letter must be prefixed with a slash
    if (pathName[0] != '/') {
      pathName = '/' + pathName;
    }

    return encodeURI("file://" + pathName);
  }

}
