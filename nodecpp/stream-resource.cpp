#include "stream-resource.h"

namespace nodecpp {

  int StreamResource::DoTryWrite(uv_buf_t** /*bufs*/, size_t* /*count*/) {
    return 0;
  }

  const char* StreamResource::Error() const {
    return nullptr;
  }

  void StreamResource::ClearError() {

  }

}