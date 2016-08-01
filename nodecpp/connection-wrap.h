#pragma once
#include "stream-wrap.h"

namespace nodecpp {
  template <typename WrapType, typename UVType>
  class ConnectionWrap : public StreamWrap {
  public:
    UVType* UVHandle() {
      return &handle_;
    }

    static void OnConnection(uv_stream_t* handle, int status);

  protected:
    ConnectionWrap(
      ProviderType provider,
      AsyncWrap* parent);
    ~ConnectionWrap() = default;

    UVType handle_;
  };

}