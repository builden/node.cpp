#ifndef __HOOKER_H__
#define __HOOKER_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class Detours : public Singleton<Detours> {
  public:
    /**
     * @originFuncPtr {point-point} ԭʼ����ָ���ָ�룬hook��ԭʼ�����ĵ�ַ�ᷢ���仯
     * @return {bool} �Ƿ��ȡ����ԭʼ������ַ
     */
    bool addFuncInfo(const string& moduleName, const string& funcName, void **originFuncPtr, void *newFunc);

    void hook();
    void unhook();

    bool execWithDll(const string& command, const string& dllPath);

  private:
    struct FuncInfo {
      string moduleName;
      string funcName;
      void **originFuncPtr;
      void *newFunc;
    };

    vector<FuncInfo> vecFuncInfo_;
  };

  extern Detours &detours;
}

#endif // !__HOOKER_H__
