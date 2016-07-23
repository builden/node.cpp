#include "write-wrap.h"

namespace nodecpp {
  void WriteWrap::Dispose() {
    this->~WriteWrap();
    delete[] reinterpret_cast<char*>(this);
  }
}