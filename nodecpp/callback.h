#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "nodecpp-def.h"

namespace nodecpp {
  template<typename callback_t>
  class Callback {
  public:
    Callback(callback_t cb, void *data = nullptr) : cb_(cb), data_(data) {}
    
    template<typename ... Args>
    void invoke(Args& ... args) {
      cb_(std::forward<Args>(args)...);
    }

    void* getData() const { return data_; }

  private:
    callback_t cb_;
    void* data_;
  };
}

#endif // !__CALLBACK_H__