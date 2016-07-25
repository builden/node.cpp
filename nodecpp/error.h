#ifndef __ERROR_H__
#define __ERROR_H__

#include "nodecpp-def.h"
#include <exception>

namespace nodecpp {
  class Error : public std::exception {
  public:
    Error() : Error(0) {}
    Error(int err) : err_(err) {}
    Error(int err, const string& msg);
    Error(const string& msg);
    ~Error() = default;

  public:
    operator bool() const { return err_ < 0; }
    int code() const { return err_; }
    const char* name() const;
    const char* str() const;

    virtual const char* what() const throw() {
      return str();
    }
  private:
    int err_;
    bool isCustom = false;
    string customMsg;
  };
}

#endif // !__ERROR_H__
