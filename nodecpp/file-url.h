#ifndef __FILE_URL_H__
#define __FILE_URL_H__
#include "nodecpp-def.h"

namespace nodecpp {
  using std::string;

  string fileUrl(const string& str, bool isResolve = true);
}

#endif // !__FILE_URL_H__
