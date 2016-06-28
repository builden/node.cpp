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

    // ���ص�һ�����ҵ���ip
    void lookup(const string& hostname, LookupCb_t cb);
    
    // address��ip
    // �� getnameinfo ��������ĵ�ַ�Ͷ˿�Ϊ�����ͷ���
    void lookupService(const string& address, int port, LookupService_t cb);

    // ����ip���飬����ipv4, ipv6
    void resolve(const string& hostname, ResolveCb_t cb);
    void resolve4(const string& hostname, ResolveCb_t cb);
    void resolve6(const string& hostname, ResolveCb_t cb);
    void resolveCname(const string& hostname, ResolveCb_t cb);

    // ����getHostByAddr������ip������������
    // void reverse(const string& ip, ResolveCb_t cb);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern Dns& dns;
}