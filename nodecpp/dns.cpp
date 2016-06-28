#include "dns.h"
#include <uv.h>
#include "callback.h"
#include "process.h"

namespace nodecpp {
  using ResolveCb = Callback<ResolveCb_t>;
  using LookupCb = Callback<LookupCb_t>;
  class Dns::impl {
  public:
    void lookup(const string& hostname, LookupCb_t cb) {
      resolve(hostname, [cb](const Error& err, svec_t addresses) {
        string address;
        if (!addresses.empty()) address = addresses[0];
        cb(err, address);
      });
    }

    void resolve(const string& hostname, ResolveCb_t cb) {
      struct addrinfo hints;
      hints.ai_family = PF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_flags = 0;

      uv_getaddrinfo_t* resolver = new uv_getaddrinfo_t;
      resolver->data = new ResolveCb(cb);
      int r = uv_getaddrinfo(uv_default_loop(), resolver, on_resolved, hostname.c_str(), "6667", &hints);

      if (r) {
        process.nextTick([r, cb]() {
          cb(Error(r), svec_t{});
        });
        delete resolver;
      }
    }

  private:
    static void on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo *res) {
      auto cb = (ResolveCb *)resolver->data;
      delete resolver;
      if (status < 0) {
        cb->invoke(Error(status), svec_t{});
        return;
      }

      svec_t rst;
      char addr[17] = { '\0' };
      auto next = res;
      do {
        uv_ip4_name((struct sockaddr_in*) next->ai_addr, addr, 16);
        rst.emplace_back(addr);
        next = next->ai_next;
      } while (next != nullptr);
      cb->invoke(Error(status), rst);
    }

  private:
    
  };

  Dns::Dns() : pimpl(new impl) {}
  Dns::~Dns() {}

  void Dns::lookup(const string& hostname, LookupCb_t cb) {
    pimpl->lookup(hostname, cb);
  }

  void Dns::lookupService(const string& /*address*/, int /*port*/, LookupService_t /*cb*/) {

  }

  void Dns::resolve(const string& hostname, ResolveCb_t cb) {
    pimpl->resolve(hostname, cb);
  }

  Dns& dns = Dns::instance();
}