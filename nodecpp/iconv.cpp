#include "iconv.h"
#include <windows.h>
#include <codecvt>
using std::wstring_convert;
using std::codecvt;
using std::mbstate_t;
using std::codecvt_utf8;

namespace nodecpp {

  string Iconv::decode(const Buffer& buf, const string& encoding, bool stripBOM /*= false*/) {
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

  Buffer Iconv::encode(const string& str, const string& encoding, bool stripBOM /*= false*/) {
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

  Buffer Iconv::encode(const wstring& str, const string& encoding, bool stripBOM /*= false*/) {
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
}