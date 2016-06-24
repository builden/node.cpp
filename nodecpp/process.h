#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "nodecpp-def.h"
#include "singleton.h"
#include <string>
#include "json11.hpp"

namespace nodecpp {
  using std::string;
  using std::function;
  using json11::Json;
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
    Json env;
    string platform = "win32";

  private:
    void initArgv();
    void initEnv();

  private:
    string cwd_;
  };

  extern Process &process;
}

#endif // !__PROCESS_H__
