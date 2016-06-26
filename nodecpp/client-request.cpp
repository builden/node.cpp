#include "client-request.h"

namespace nodecpp {
  class ClientRequest::impl {
  public:
  };

  ClientRequest::ClientRequest() : pimpl(new impl) {}
  ClientRequest::~ClientRequest() {}
}