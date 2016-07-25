#pragma once
#include "nodecpp-def.h"
#include "error.h"
#include "buffer.h"

namespace nodecpp {
  static int kReadFileBufferLength = 8 * 1024;
  class ReadFileContext {
  public:
    int fd = 0;
    bool isUserFd = false;
    int size = 0;
    function<void(const Error&, const Buffer&)> callback = nullptr;
    Buffer buffers;
    Buffer buffer;
    int pos = 0;
    int encoding = 0;
    int err = 0;

    void read();
    void close();

  private:
//     void readFileAfterOpen(const Error& err, int fd);
//     void readFileAfterStat(const Error& err, const Stats& st);
//     void readFileAfterRead(const Error& err, int bytesRead);
//     void readFileAfterClose(const Error& err);
  };
}