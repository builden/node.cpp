#include "underscore.string.h"
#include "core-math.h"
#include "iconv.h"
#include <regex>
#include "fmt/format.h"
#include "std-regex-ex.h"

namespace nodecpp {

  string UnderscoreString::slice(const string& str, int start) {
    int end = str.length();
    return slice(str, start, end);
  }

  string UnderscoreString::slice(const string& str, int start, int end) {
    if (start < 0) start += str.length();
    if (end < 0) end += str.length();
    int len = end - start;
    if (start < 0) start = 0;
    if (len <= 0) return "";
    return str.substr(start, end - start);
  }

  wstring UnderscoreString::slice(const wstring& wstr, int start) {
    int end = wstr.length();
    return slice(wstr, start, end);
  }

  wstring UnderscoreString::slice(const wstring& wstr, int start, int end) {
    if (start < 0) start += wstr.length();
    if (end < 0) end += wstr.length();
    int len = end - start;
    if (start < 0) start = 0;
    if (len <= 0) return L"";
    return wstr.substr(start, end - start);
  }

  svec_t UnderscoreString::split(const string& str, const string& separator /*= " "*/, int limit /*= -1*/) {
    svec_t v;
    string::size_type pos1, pos2;
    pos2 = str.find(separator);
    pos1 = 0;
    while (string::npos != pos2) {
      v.push_back(str.substr(pos1, pos2 - pos1));
      if (limit > 0 && static_cast<int>(v.size()) == limit) return v;

      pos1 = pos2 + separator.size();
      pos2 = str.find(separator, pos1);
    }
    if (pos1 != str.length()) v.push_back(str.substr(pos1));
    return v;
  }

  string UnderscoreString::join(const svec_t& svec, const string& separator /*= ""*/) {
    string v;
    size_t idx = 0, len = svec.size();
    for (auto &str : svec) {
      v += str;
      if (++idx != len) v += separator;
    }
    return v;
  }

  bool UnderscoreString::startsWith(const string& str, const string& starts, size_t position /*= 0*/) {
    position = Math.min(position, str.length());
    return str.find(starts) == position;
  }

  bool UnderscoreString::endsWith(const string& str, const string& ends, size_t position) {
    position = Math.min(position, str.length()) - ends.length();
    return position >= 0 && str.rfind(ends) == position;
  }


  bool UnderscoreString::endsWith(const string& str, const string& end) {
    return endsWith(str, end, str.length());
  }

  string UnderscoreString::ltrim(const string& str, const string& characters /*= " \t\r\n"*/) {
    string _str = str;
    return _str.erase(0, _str.find_first_not_of(characters));
  }

  string UnderscoreString::rtrim(const string& str, const string& characters /*= " \t\r\n"*/) {
    string _str = str;
    return _str.erase(_str.find_last_not_of(characters) + 1);
  }

  string UnderscoreString::trim(const string& str, const string& characters /*= " \t\r\n"*/) {
    string _str = str;
    _str.erase(0, _str.find_first_not_of(characters));
    return _str.erase(_str.find_last_not_of(characters) + 1);
  }


  string UnderscoreString::repeat(const string& str, int n /*= 1*/) {
    string v;
    while (n-- > 0) {
      v += str;
    }
    return v;
  }

  string UnderscoreString::lpad(const string& str, int length, const string& padStr /*= " "*/) {
    string padChar = " ";
    if (padStr.length() > 0) padChar = slice(padStr, 0, 1);
    int padLen = length - str.length();
    return repeat(padChar, padLen) + str;
  }

  string UnderscoreString::rpad(const string& str, int length, const string& padStr /*= " "*/) {
    string padChar = " ";
    if (padStr.length() > 0) padChar = slice(padStr, 0, 1);
    int padLen = length - str.length();
    return str + repeat(padChar, padLen);
  }

  string UnderscoreString::pad(const string& str, int length, const string& padStr /*= " "*/) {
    string padChar = " ";
    if (padStr.length() > 0) padChar = slice(padStr, 0, 1);
    int padLen = length - str.length();
    return repeat(padChar, Math.ceil((double)padLen / 2)) + str + repeat(padChar, Math.floor((double)padLen / 2));
  }

  string UnderscoreString::toLower(const string& str) {
    string v = str;
    transform(v.begin(), v.end(),
      v.begin(), ::tolower);
    return v;
  }

  string UnderscoreString::toUpper(const string& str) {
    string v = str;
    transform(v.begin(), v.end(),
      v.begin(), ::toupper);
    return v;
  }

  int UnderscoreString::indexOf(const string& str, const string& searchValue, int fromIndex /*= 0*/) {
    return str.find(searchValue, fromIndex);
  }

  int UnderscoreString::lastIndexOf(const string& str, const string& searchValue, int fromIndex /*= string::npos*/) {
    return str.rfind(searchValue, fromIndex);
  }

  string UnderscoreString::strLeft(const string& str, const string& sep) {
    int pos = sep.empty() ? -1 : indexOf(str, sep);
    return pos > 0 ? slice(str, 0, pos) : str;
  }

  string UnderscoreString::strRight(const string& str, const string& sep) {
    int pos = sep.empty() ? -1 : indexOf(str, sep);
    return pos > 0 ? slice(str, pos + sep.length(), str.length()) : str;
  }

  string UnderscoreString::strLeftBack(const string& str, const string& sep) {
    int pos = lastIndexOf(str, sep);
    return pos > 0 ? slice(str, 0, pos) : str;
  }

  string UnderscoreString::strRightBack(const string& str, const string& sep) {
    int pos = lastIndexOf(str, sep);
    return pos > 0 ? slice(str, pos + sep.length(), str.length()) : str;
  }

  bool UnderscoreString::includes(const string& str, const string& searchString, int position /*= 0*/) {
    return str.find(searchString, position) != -1;
  }

  string UnderscoreString::truncate(const string& str, size_t length, const string& trunctateStr /*= "..."*/) {
    return str.length() > length ? slice(str, 0, length) + trunctateStr : str;
  }


  string UnderscoreString::surround(const string& str, const string& wrapper) {
    svec_t svec = { wrapper, str, wrapper };
    return join(svec);
  }

  string UnderscoreString::quote(const string& str, const char quoteChar /*= '"'*/) {
    std::string quoteStr;
    quoteStr += quoteChar;
    return surround(str, quoteStr);
  }

  string UnderscoreString::unquote(const string& str, const char quoteChar /*= '"'*/) {
    std::string quoteStr;
    quoteStr += quoteChar;
    if (str[0] == quoteChar && str[str.length() - 1] == quoteChar) {
      return slice(str, 1, str.length() - 1);
    }
    else {
      return str;
    }
  }

  string UnderscoreString::replace(const string& str, const string& find, const string& replace, bool ignorecase /*= false*/) {
    string findStr = ignorecase ? toLower(str) : str;
    string subject = str;

    string search = ignorecase ? toLower(find) : find;
    size_t pos = 0;
    if ((pos = findStr.find(search, pos)) != std::string::npos) {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
    }
    return subject;
  }

  string UnderscoreString::replaceAll(const string& str, const string& find, const string& replace, bool ignorecase /*= false*/) {
    string findStr = ignorecase ? toLower(str) : str;
    string subject = str;

    string search = ignorecase ? toLower(find) : find;
    size_t pos = 0;
    while ((pos = findStr.find(search, pos)) != std::string::npos) {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
      findStr = ignorecase ? toLower(subject) : subject;
    }
    return subject;
  }

  string UnderscoreString::replaceTmpl(const string& str, json j) {
    string rst = trim(str);
    std::regex replacePattern("\\$([^\\$]+?)\\$");
    rst = std::regex_replace(rst, replacePattern, [this, j](const std::smatch &m) {
      if (m.size() == 2) {
        string m1 = m[1].str();
        if (j.find(m1) != j.end()) return j[m1].dump();
      }
      return m[0].str();
    });
    return rst;
  }

  string UnderscoreString::reverse(const string& str) {
    wstring wstr = iconv.stow(str);
    std::reverse(wstr.begin(), wstr.end());
    return iconv.wtos(wstr);
  }

  string UnderscoreString::capitalize(const string& str, bool lowercaseRest /*= false*/) {
    string remainingChars = !lowercaseRest ? slice(str, 1) : toLower(slice(str, 1));
    return toUpper(string(1, str[0])) + remainingChars;
  }

  string UnderscoreString::decapitalize(const string& str) {
    string rst(1, char(tolower(str[0])));
    return rst + slice(str, 1);
  }

  string UnderscoreString::camelize(const string& str, bool decap /*= false*/) {
    string rst = trim(str);
    std::regex replacePattern(R"([-_\s]+(.)?)");
    rst = std::regex_replace(rst, replacePattern, [this](const std::smatch &m) {
      return (m.size() == 2) ? toUpper(m[1].str()) : "";
    });

    return decap ? decapitalize(rst) : rst;
  }

  UnderscoreString &s = UnderscoreString::instance();
}

