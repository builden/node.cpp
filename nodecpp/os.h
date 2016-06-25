#ifndef __OS_H__
#define __OS_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class Os: public Singleton<Os> {
  public:
    string platform();
    string tmpdir();
    string homedir();
  };

  extern Os& os;
}

#endif // !__OS_H__
