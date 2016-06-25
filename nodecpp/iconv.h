/**
 *
 */
#ifndef __ICONV_H__
#define __ICONV_H__
#include "buffer.h"
#include "singleton.h"

namespace nodecpp {
  class Iconv : public Singleton<Iconv> {
  public:
    string decode(const Buffer& buf, const string& encoding, bool stripBOM = false);

    // str to buffer
    Buffer encode(const string& str, const string& encoding, bool stripBOM = false);
    Buffer encode(const wstring& str, const string& encoding, bool stripBOM = false);

    wstring strToWstr(const string& s);
    string wstrToStr(const wstring& ws);

    wstring utf8ToWstr(const string& s);
    string wstrToUtf8(const wstring& ws);

    string utf8ToStr(const string& s);
    string strToUtf8(const string& s);

    uint32_t charCodeAt(const string& ansiStr, uint32_t idx);
    uint32_t charCodeAt(const wstring& ws, uint32_t idx);
  };

  extern Iconv &iconv;
}

#endif // !__ICONV_H__
