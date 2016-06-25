#ifndef __HOOKER_H__
#define __HOOKER_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class Hooker : public Singleton<Hooker> {
  public:
    /**
     * @originFuncPtr {point-point} 原始函数指针的指针，hook后原始函数的地址会发生变化
     * @return {bool} 是否获取到了原始函数地址
     */
    bool addFuncInfo(const string& moduleName, const string& funcName, void **originFuncPtr, void *newFunc);

    void hook();
    void unhook();

  private:
    struct FuncInfo {
      string moduleName;
      string funcName;
      void **originFuncPtr;
      void *newFunc;
    };

    vector<FuncInfo> vecFuncInfo_;
  };

  extern Hooker &hooker;
}

#endif // !__HOOKER_H__
