#ifndef __QUERYSTRING_H__
#define __QUERYSTRING_H__

#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class Querystring : public Singleton<Querystring> {
  public:
    json parse(const string& str, const string& sep = "&", const string& eq = "=");
    string stringify(json& obj, const string& sep = "&", const string& eq = "=");
    Querystring();

  private:
    string escape(const string& str);
    string encode(const string& str);
    string unescape(const string& str);
    string decodeStr(const string& str);
    string stringifyPrimitive(json& obj);

  private:
    svec_t hexTable;
  };

  extern Querystring &qs;
}


#endif // !__QUERYSTRING_H__
