#include "error.h"
#include <uv.h>

namespace nodecpp {

  const char* Error::name() const {
    return uv_err_name(err_);
  }

  const char* Error::str() const {
    return uv_strerror(err_);
  }

}