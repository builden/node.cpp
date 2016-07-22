#pragma once
#include "async-wrap.h"

namespace nodecpp {
  class HandleWrap : public AsyncWrap {
  public:
    void close();

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
  };
}