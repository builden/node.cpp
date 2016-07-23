#include "handle-wrap.h"

namespace nodecpp {

  void HandleWrap::close(CloseCb_t cb /*= nullptr*/) {
    if (state_ != kInitialized) return;

    uv_close(handle__, onClose);
    state_ = kClosing;

    if (cb != nullptr) {
      closeCb_ = cb;
      state_ = kClosingWithCallback;
    }
  }

  HandleWrap::HandleWrap(uv_handle_t* handle, AsyncWrap::ProviderType provider, AsyncWrap* parent /*= nullptr*/) 
    : AsyncWrap(provider, parent), 
      state_(kInitialized),
      handle__(handle) {
    handle__->data = this;
  }

  HandleWrap::~HandleWrap() {

  }

  void HandleWrap::onClose(uv_handle_t* handle) {
    HandleWrap* wrap = static_cast<HandleWrap*>(handle->data);
    wrap->state_ = kClosed;

    if (wrap->closeCb_) wrap->closeCb_();
    delete wrap;
  }

}
