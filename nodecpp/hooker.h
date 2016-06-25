#ifndef __HOOKER_H__
#define __HOOKER_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class Hooker : public Singleton<Hooker> {
  public:
    /**
     * @originFuncPtr {point-point} ԭʼ����ָ���ָ�룬hook��ԭʼ�����ĵ�ַ�ᷢ���仯
     * @return {bool} �Ƿ��ȡ����ԭʼ������ַ
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
