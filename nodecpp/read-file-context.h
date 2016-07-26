#pragma once
#include "nodecpp-def.h"
#include "error.h"
#include "buffer.h"
#include "stats-def.h"

namespace nodecpp {
  static int kReadFileBufferLength = 8 * 1024;
  using CtxCb_t = function<void(const Error&, const Buffer&)>;
  class ReadFileContext {
  public:
    ReadFileContext(CtxCb_t cb);
    int fd_ = 0;
    bool isUserFd_ = false;
    uint32_t size_ = 0;
    CtxCb_t callback_ = nullptr;
    vector<Buffer> buffers_;
    Buffer buffer_;
    uint32_t pos_ = 0;
    int encoding_ = 0;
    Error err_;

    void read();
    void close(const Error& err);

    void readFileAfterOpen(const Error& err, int fd);
    void readFileAfterStat(const Error& err, const Stats& st);
    void readFileAfterRead(const Error& err, int bytesRead);
    void readFileAfterClose(const Error& err);
  };
}