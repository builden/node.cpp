#pragma once
#include "nodecpp-def.h"
#include "singleton.h"
#include <g3log/g3log.hpp>

namespace nodecpp {
  class G3log : public Singleton<G3log> {
  public:
    G3log();
    void init(const string& logPrefix, const string& logPath, int logLevel = 0);
    void finalize();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern G3log& g3log;
}