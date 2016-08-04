#ifndef __CHILD_PROCESS_H__
#define __CHILD_PROCESS_H__
#include "nodecpp-def.h"
#include "singleton.h"
#include "child-process.h"

namespace nodecpp {
  class OuterChildProcess : public Singleton<OuterChildProcess> {
  public:
    bool exec(const string& command);

    shared_ptr<ChildProcess> spawn(const string& command, json options);
    shared_ptr<ChildProcess> spawn(const string& command, const svec_t& args = {}, json options = json({}));

  private:
    json normalizeSpawnArguments(const string& file, const svec_t& args, json options);
  };

  extern OuterChildProcess &child_process;
}

#endif // !__CHILD_PROCESS_H__
