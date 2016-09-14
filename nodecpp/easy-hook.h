#pragma once
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class EasyHook : public Singleton<EasyHook> {
  public:
    EasyHook();

    bool addFuncInfo(const string& moduleName, const string& funcName, void *hookFunc);

    void hook();
    void unhook();

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern EasyHook& easyhook;
}
