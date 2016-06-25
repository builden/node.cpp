#pragma once
#include "nodecpp-def.h"
#include "error.h"
#include "buffer.h"

namespace nodecpp {
  using ReadCb_t = function<void(const Error&, const Buffer&)>;
  using WriteCb_t = function<void(const Error&)>;

  class FsWrap {
  public:
    FsWrap(int fd);
    ~FsWrap();

    void readFile(ReadCb_t cb);
    void writeFile(const Buffer& buf, WriteCb_t cb);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };
}
