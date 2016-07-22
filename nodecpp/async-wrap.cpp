#include "async-wrap.h"

namespace nodecpp {

  AsyncWrap::AsyncWrap(ProviderType provider, AsyncWrap* /*parent*/ /*= nullptr*/) 
    : BaseObject()
    , bits_(static_cast<uint32_t>(provider) << 1) {
    bits_ |= 1;
  }

  AsyncWrap::~AsyncWrap() {

  }

  nodecpp::AsyncWrap::ProviderType AsyncWrap::provider_type() const {
    return static_cast<ProviderType>(bits_ >> 1);
  }

}