#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  using NextTickCb_t = function<void(void)>;

  class Process : public Singleton<Process> {
  public:
    Process();
    string cwd();

    void nextTick(NextTickCb_t cb);
    string getEnv(const string& envVar);

  public:
    string execPath;
    unsigned int pid;
    svec_t argv;
    json env;
    string platform = "win32";
    string arch;
    string version;
    json versions;

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern Process &process;
}

#endif // !__PROCESS_H__
