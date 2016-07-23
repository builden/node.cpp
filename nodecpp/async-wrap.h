#pragma once
#include "base-object.h"

namespace nodecpp {
#define NODE_ASYNC_PROVIDER_TYPES(V)                                          \
  V(NONE)                                                                     \
  V(CRYPTO)                                                                   \
  V(FSEVENTWRAP)                                                              \
  V(FSREQWRAP)                                                                \
  V(GETADDRINFOREQWRAP)                                                       \
  V(GETNAMEINFOREQWRAP)                                                       \
  V(HTTPPARSER)                                                               \
  V(JSSTREAM)                                                                 \
  V(PIPEWRAP)                                                                 \
  V(PIPECONNECTWRAP)                                                          \
  V(PROCESSWRAP)                                                              \
  V(QUERYWRAP)                                                                \
  V(SHUTDOWNWRAP)                                                             \
  V(SIGNALWRAP)                                                               \
  V(STATWATCHER)                                                              \
  V(TCPWRAP)                                                                  \
  V(TCPCONNECTWRAP)                                                           \
  V(TIMERWRAP)                                                                \
  V(TLSWRAP)                                                                  \
  V(TTYWRAP)                                                                  \
  V(UDPWRAP)                                                                  \
  V(UDPSENDWRAP)                                                              \
  V(WRITEWRAP)                                                                \
  V(ZLIB)

  class AsyncWrap : public BaseObject {
  public:
    enum ProviderType {
#define V(PROVIDER)                                                           \
    PROVIDER_ ## PROVIDER,
      NODE_ASYNC_PROVIDER_TYPES(V)
#undef V
    };

    inline AsyncWrap(ProviderType provider, AsyncWrap* parent = nullptr)
      : BaseObject(), bits_(static_cast<uint32_t>(provider) << 1), parent_(parent){
      bits_ |= 1;
    }

    inline virtual ~AsyncWrap() {}

    inline ProviderType provider_type() const {
      return static_cast<ProviderType>(bits_ >> 1);
    }

  private:
    uint32_t bits_;
    AsyncWrap* parent_;
  };
}