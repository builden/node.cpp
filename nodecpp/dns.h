#pragma once
#include "nodecpp-def.h"
#include "singleton.h"
#include "error.h"

namespace nodecpp {
  using LookupCb_t = function<void(const Error&, const string& address)>;
  using LookupService_t = function<void(const Error&, const string&, const string&)>;
  using ResolveCb_t = function<void(const Error&, const svec_t&)>;

  class Dns : public Singleton<Dns> {
  public:
    Dns();
    ~Dns();

    // 返回第一个查找到的ip
    void lookup(const string& hostname, LookupCb_t cb);
    
    // address是ip
    // 用 getnameinfo 解析传入的地址和端口为域名和服务
    void lookupService(const string& address, int port, LookupService_t cb);

    // 返回ip数组，包含ipv4, ipv6
    void resolve(const string& hostname, ResolveCb_t cb);
    void resolve4(const string& hostname, ResolveCb_t cb);
    void resolve6(const string& hostname, ResolveCb_t cb);
    void resolveCname(const string& hostname, ResolveCb_t cb);

    // 调用getHostByAddr，根据ip返回域名数组
    // void reverse(const string& ip, ResolveCb_t cb);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern Dns& dns;
}