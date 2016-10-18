/**
 * BOM
 * UTF-8	0xEF 0xBB 0xBF
 * UTF-16 BE	0xFE 0xFF
 * UTF-16 LE	0xFF 0xFE
 * UTF-32 BE	0x00 0x00 0xFE 0xFF
 * UTF-32 LE	0xFF 0xFE 0x00 0x00
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

    // string to wstring
    wstring stow(const string& s);
    // wstring to string
    string wtos(const wstring& ws);

    // utf8 to wstring
    wstring u8tow(const string& s);
    // wstring to utf8
    string wtou8(const wstring& ws);

    // utf8 to string
    string u8tos(const string& s);
    // string to utf8
    string stou8(const string& s);

    uint32_t charCodeAt(const string& ansiStr, uint32_t idx);
    uint32_t charCodeAt(const wstring& ws, uint32_t idx);

    uint32_t u8Length(const string& u8Str);
    uint32_t sLength(const string& str);
  };

  extern Iconv &iconv;
}

#endif // !__ICONV_H__
