#ifndef __URL_H__
#define __URL_H__

#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {

  class UrlObj {
  public:
    string protocol;
    bool slashes = false;
    string auth;
    string host;
    string port;
    string hostname;
    string hash;
    string search;
    string query;
    string pathname;
    string path;
    string href;
  };

  class Url : public Singleton<Url> {
  public:
    string format(const UrlObj& urlObj);
    UrlObj parse(const string& urlStr, bool slashesDenoteHost = false);
    string resolve(const string& from, const string& to);

    Url();

  private:
    void parseHost(UrlObj& obj);
    string validateHostname(UrlObj& obj, const string& rest, const string& hostname);
    string autoEscapeStr(const string& rest);
    string formatHref(UrlObj& obj);
    string encodeAuth(const string& str);

  private:
    svec_t hexTable;
  };

  extern Url &url;
}

#endif // !__URL_H__
