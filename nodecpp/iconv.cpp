#include "iconv.h"
#include <windows.h>
#include <codecvt>
#include <comutil.h>

#pragma comment(lib, "comsuppw.lib")

using std::wstring_convert;
using std::codecvt;
using std::mbstate_t;
using std::codecvt_utf8;

namespace nodecpp {

  string Iconv::decode(const Buffer& buf, const string& encoding, bool /*stripBOM*/ /*= false*/) {
    if (encoding == "utf-8" || encoding == "utf8") {
      int nwLen = MultiByteToWideChar(CP_UTF8, 0, buf.data(), -1, NULL, 0);

      wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
      memset(pwBuf, 0, nwLen * 2 + 2);

      MultiByteToWideChar(CP_UTF8, 0, buf.data(), buf.length(), pwBuf, nwLen);

      int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

      char * pBuf = new char[nLen + 1];
      memset(pBuf, 0, nLen + 1);

      WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

      std::string retStr = pBuf;

      delete[]pBuf;
      delete[]pwBuf;

      pBuf = NULL;
      pwBuf = NULL;

      return retStr;
    }
    return "";
  }

  Buffer Iconv::encode(const string& str, const string& encoding, bool /*stripBOM*/ /*= false*/) {
    if (encoding == "utf-8" || encoding == "utf8") {
      int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

      wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
      ZeroMemory(pwBuf, nwLen * 2 + 2);

      ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

      int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

      char * pBuf = new char[nLen + 1];
      ZeroMemory(pBuf, nLen + 1);

      ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

      std::string retStr(pBuf);

      delete[]pwBuf;
      delete[]pBuf;

      pwBuf = NULL;
      pBuf = NULL;

      return Buffer(retStr);
    }
    Buffer buf((size_t)0);
    return buf;
  }

  Buffer Iconv::encode(const wstring& /*str*/, const string& /*encoding*/, bool /*stripBOM*/ /*= false*/) {
    Buffer buf((size_t)0);
    return buf;
  }

  wstring Iconv::strToWstr(const string& s) {
    wstring_convert<codecvt<wchar_t, char, mbstate_t>> converter(new codecvt<wchar_t, char, mbstate_t>("CHS"));

    return converter.from_bytes(s);
  }

  string Iconv::wstrToStr(const wstring& ws) {
    wstring_convert<codecvt<wchar_t, char, mbstate_t>> converter(new codecvt<wchar_t, char, mbstate_t>("CHS"));

    return converter.to_bytes(ws);
  }

  wstring Iconv::utf8ToWstr(const string& s) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(s);
  }

  string Iconv::wstrToUtf8(const wstring& ws) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(ws);
  }

  string Iconv::utf8ToStr(const string& s) {
    return wstrToStr(utf8ToWstr(s));
  }

  string Iconv::strToUtf8(const string& s) {
    return wstrToUtf8(strToWstr(s));
  }

  uint32_t Iconv::charCodeAt(const string& ansiStr, uint32_t idx) {
    return strToWstr(ansiStr)[idx];
  }

  uint32_t Iconv::charCodeAt(const wstring& ws, uint32_t idx) {
    return ws[idx];
  }

  Iconv &iconv = Iconv::instance();

  uint32_t Iconv::utf8Length(const string& u8Str) {
    uint32_t len = 0;
    auto begin = u8Str.begin(), end = u8Str.end();
    while (begin != end) {
      unsigned char c = *begin;
      int n = 0;
      if ((c & 0x80) == 0)    n = 1;
      else if ((c & 0xE0) == 0xC0) n = 2;
      else if ((c & 0xF0) == 0xE0) n = 3;
      else if ((c & 0xF8) == 0xF0) n = 4;
      else throw std::runtime_error("utf8_length: invalid UTF-8");

      if (end - begin < n) {
        throw std::runtime_error("utf8_length: string too short");
      }
      for (int i = 1; i < n; ++i) {
        if ((begin[i] & 0xC0) != 0x80) {
          throw std::runtime_error("utf8_length: expected continuation byte");
        }
      }
      len += 1;
      begin += n;
    }
    return len;
  }

  uint32_t Iconv::ansiLength(const string& str) {
    return _bstr_t(str.c_str()).length();
  }

}