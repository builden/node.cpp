#ifndef __CORE_H__
#define __CORE_H__
#include "nodecpp-def.h"

namespace nodecpp {
  using std::string;

  string encodeURI(const string& str);
  string decodeURI(const string& str);
  string encodeURIComponent(const string& str);
  string decodeURIComponent(const string& str);
  string escape(const string& str);
  string unescape(const string& str);
}

#endif // !__CORE_H__
