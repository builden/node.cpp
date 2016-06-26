#ifndef __PATH_H__
#define __PATH_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class Path : public Singleton<Path> {
  public:
    Path();

    string basename(const string& p, const string& ext = "");
    string dirname(const string& p);
    string extname(const string& p);
    string join(const string& path1
      , const std::string& path2
      , const std::string& path3 = ""
      , const std::string& path4 = ""
      , const std::string& path5 = "");
    string normalize(const string& p);
    bool isAbsolute(const string& p);

    /**
     * Is similar to:
     *   cd path1
     *   cd path2
     *   cd path3
     *   cd path4
     *   pwd
     */
    string resolve(const string& path0
      , const string& path1 = ""
      , const string& path2 = ""
      , const string& path3 = "");
  public:
    string sep;

  private:
    string normalizeStringWin32(const string& p, bool allowAboveRoot);
  };

  extern Path &path;
}

#endif // !__PATH_H__
