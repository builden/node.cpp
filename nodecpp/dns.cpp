#include "dns.h"
#include <uv.h>
#include "callback.h"
#include "process.h"
#include "req-wrap.h"
#include "req-wrap-inl.h"

namespace nodecpp {
  class GetAddrInfoReqWrap : public ReqWrap<uv_getaddrinfo_t> {
  public:
    GetAddrInfoReqWrap(CallbackBase* cb) : ReqWrap(cb, AsyncWrap::PROVIDER_GETADDRINFOREQWRAP) {};

    size_t self_size() const override { return sizeof(*this); }
  };

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
      int family = PF_INET;
      GetAddrInfoReqWrap* req_wrap = new GetAddrInfoReqWrap(new CallbackWrap<ResolveCb_t>(cb));

      struct addrinfo hints;
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = family;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = 0;

      int err = uv_getaddrinfo(uv_default_loop(),
        &req_wrap->req_,
        AfterGetAddrInfo,
        hostname.c_str(),
        nullptr,
        &hints);
      req_wrap->Dispatched();

      if (err) {
        delete req_wrap;
        process.nextTick([err, cb]() {
          cb(Error(err), svec_t{});
        });
      }

      /*
      struct addrinfo hints;
      hints.ai_family = PF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_flags = 0;

      uv_getaddrinfo_t* resolver = new uv_getaddrinfo_t;
      resolver->data = new ResolveCb(cb);
      int r = uv_getaddrinfo(uv_default_loop(), resolver, AfterGetAddrInfo, hostname.c_str(), "6667", &hints);

      if (r) {
        process.nextTick([r, cb]() {
          cb(Error(r), svec_t{});
        });
        delete resolver;
      }*/
    }

  private:
    static void AfterGetAddrInfo(uv_getaddrinfo_t* req, int status, struct addrinfo *res) {
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
      req_wrap->invoke(Error(status), argv);
      delete req_wrap;
/*
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
      cb->invoke(Error(status), rst);*/
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