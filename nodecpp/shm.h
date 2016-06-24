#ifndef __SHM_H__
#define __SHM_H__
#include "nodecpp-def.h"
#include <windows.h>

// ¹²ÏíÄÚ´æ
namespace nodecpp {
  using std::string;
  class Shm {
  public:
    ~Shm();
    // server
    bool create(const string& name, uint32_t size);

    // client
    bool open(const string& name);

    const char* read();
    bool write(const char* buf, uint32_t size);

    void close();
  private:
    char * buf_ = nullptr;
    HANDLE fileMapping_ = NULL;
  };
}


#endif // !__SHM_H__
