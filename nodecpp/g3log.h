#pragma once
#include "nodecpp-def.h"
#include "singleton.h"
#include <g3log/g3log.hpp>

#ifdef _DEBUG
#ifdef _DLL
#pragma comment(lib, "g3logger-d.lib")
#else  // _DLL
#pragma comment(lib, "g3logger-sd.lib")
#endif // _DLL
#else  // _DEBUG
#ifdef _DLL
#pragma comment(lib, "g3logger.lib")
#else  // _DLL
#pragma comment(lib, "g3logger-s.lib")
#endif // _DLL
#endif // _DEBUG

namespace nodecpp {
  class G3log : public Singleton<G3log> {
  public:
    enum LOG_TYPE {
      DEFAULT = 0,
      JSON = 1
    };
  public:
    G3log();
    void init(const string& logPrefix, const string& logPath, LOG_TYPE logType = DEFAULT);
    void finalize();

    void setLogLevel(int level);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern G3log& g3log;
}