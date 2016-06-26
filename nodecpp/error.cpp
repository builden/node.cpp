#include "error.h"
#include <uv.h>

namespace nodecpp {

  const char* Error::name() const {
    if (isCustom) return customMsg.c_str();
    return uv_err_name(err_);
  }

  const char* Error::str() const {
    if (isCustom) return customMsg.c_str();
    return uv_strerror(err_);
  }

  Error::Error(int err, const string& msg) {
    err_ = err;
    isCustom = true;
    customMsg = msg;
  }

}