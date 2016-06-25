#ifndef __CHILD_PROCESS_H__
#define __CHILD_PROCESS_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class ChildProcess : public Singleton<ChildProcess> {
  public:
    bool exec(const string& command);
    bool execWithDll(const string& command, const string& dllPath);
  };

  extern ChildProcess &child_process;
}

#endif // !__CHILD_PROCESS_H__
