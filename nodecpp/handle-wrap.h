#pragma once
#include "nodecpp-def.h"
#include "async-wrap.h"

namespace nodecpp {
  using CloseCb_t = function<void()>;
  class HandleWrap : public AsyncWrap {
  public:
    void close(CloseCb_t = nullptr);

  protected:
    HandleWrap(
      uv_handle_t* handle,
      AsyncWrap::ProviderType provider,
      AsyncWrap* parent = nullptr);
    ~HandleWrap() override;

  private:
    enum { kInitialized, kClosing, kClosingWithCallback, kClosed } state_;
    static void onClose(uv_handle_t* handle);
    uv_handle_t* const handle__;
    CloseCb_t closeCb_;
  };
}