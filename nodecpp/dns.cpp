#include "dns.h"
#include <uv.h>
#include "process.h"
#include "req-wrap.h"

namespace nodecpp {
  class GetAddrInfoReqWrap : public ReqWrap<uv_getaddrinfo_t> {
  public:
    GetAddrInfoReqWrap(CallbackBase* cbWrap) : ReqWrap(cbWrap, AsyncWrap::PROVIDER_GETADDRINFOREQWRAP) {};

    size_t self_size() const override { return sizeof(*this); }
  };

  using ResolveCbWrap = CallbackWrap<ResolveCb_t>;
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
      int family = PF_INET;
      GetAddrInfoReqWrap* reqWrap = new GetAddrInfoReqWrap(new ResolveCbWrap(cb));

      struct addrinfo hints;
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = family;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = 0;

      int err = uv_getaddrinfo(uv_default_loop(),
        &reqWrap->req_,
        onGetAddrInfo,
        hostname.c_str(),
        nullptr,
        &hints);
      reqWrap->Dispatched();

      if (err) {
        delete reqWrap;
        process.nextTick([err, cb]() {
          cb(Error(err), svec_t{});
        });
      }
    }

  private:
    static void onGetAddrInfo(uv_getaddrinfo_t* req, int status, struct addrinfo *res) {
      GetAddrInfoReqWrap* req_wrap = static_cast<GetAddrInfoReqWrap*>(req->data);
      svec_t argv;

      if (status == 0) {
        // Success
        struct addrinfo *address;

        char ip[INET6_ADDRSTRLEN];
        const char *addr;

        // Iterate over the IPv4 responses again this time creating javascript
        // strings for each IP and filling the results array.
        address = res;
        while (address) {
          CHECK_EQ(address->ai_socktype, SOCK_STREAM);

          // Ignore random ai_family types.
          if (address->ai_family == AF_INET) {
            // Juggle pointers
            addr = reinterpret_cast<char*>(&(reinterpret_cast<struct sockaddr_in*>(
              address->ai_addr)->sin_addr));
            int err = uv_inet_ntop(address->ai_family,
              addr,
              ip,
              INET6_ADDRSTRLEN);
            if (err)
              continue;

            argv.emplace_back(ip);
          }

          // Increment
          address = address->ai_next;
        }

        // Iterate over the IPv6 responses putting them in the array.
        address = res;
        while (address) {
          CHECK_EQ(address->ai_socktype, SOCK_STREAM);

          // Ignore random ai_family types.
          if (address->ai_family == AF_INET6) {
            // Juggle pointers
            addr = reinterpret_cast<char*>(&(reinterpret_cast<struct sockaddr_in6*>(
              address->ai_addr)->sin6_addr));
            int err = uv_inet_ntop(address->ai_family,
              addr,
              ip,
              INET6_ADDRSTRLEN);
            if (err)
              continue;

            // Create JavaScript string
            argv.emplace_back(ip);
          }

          // Increment
          address = address->ai_next;
        }
      }

      uv_freeaddrinfo(res);
      auto cbWrap = (ResolveCbWrap *)(req_wrap->cbWrap_);
      if (cbWrap) cbWrap->invoke(Error(status), argv);
      delete req_wrap;
    }
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