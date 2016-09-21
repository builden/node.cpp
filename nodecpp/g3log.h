#pragma once
#include "nodecpp-def.h"
#include "singleton.h"
#include <g3log/g3log.hpp>

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