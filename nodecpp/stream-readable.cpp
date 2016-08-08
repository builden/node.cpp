#include "stream-readable.h"
#include "process.h"

namespace nodecpp {
  
  void Readable::push(const Buffer& buf) {
    // auto _buf = new Buffer(buf);
    process.nextTick([=]() {
      emit<const Buffer&>("data", buf);
    });
  }

  void Readable::push() {
    process.nextTick([this]() {
      this->emit("end");
    });
  }

}