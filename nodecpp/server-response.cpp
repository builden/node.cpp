#include "server-response.h"

namespace nodecpp {
  class ServerResponse::impl {
  public:
  };

  ServerResponse::ServerResponse() : pimpl(new impl) {}
  ServerResponse::~ServerResponse() {}
}