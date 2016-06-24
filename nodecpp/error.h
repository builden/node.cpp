#ifndef __ERROR_H__
#define __ERROR_H__

#include "nodecpp-def.h"
#include <uv.h>
#include <exception>

namespace nodecpp {
  class Error : public std::exception {
  public:
    Error() : Error(0) {}
    Error(int err) : err_(err) {}
    ~Error() = default;

  public:
    operator bool() const { return err_ < 0; }
    int code() const { return err_; }
    const char* name() const { return uv_err_name(err_); }
    const char* str() const { return uv_strerror(err_); }

    virtual const char* what() const throw() {
      return str();
    }
  private:
    int err_;
  };
}

#endif // !__ERROR_H__
