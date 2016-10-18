#ifndef __UNDERSCORE_STRING_H__
#define __UNDERSCORE_STRING_H__
#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class UnderscoreString : public Singleton<UnderscoreString> {
  public:
    string slice(const string& str, int start);
    string slice(const string& str, int start, int end);
    wstring slice(const wstring& wstr, int start);
    wstring slice(const wstring& wstr, int start, int end);
    svec_t split(const string& str, const string& separator = " ", int limit = -1);
    string join(const svec_t& svec, const string& separator = "");

    bool startsWith(const string& str, const string& starts, size_t position = 0);
    bool endsWith(const string& str, const string& end);
    bool endsWith(const string& str, const string& ends, size_t position);

    string ltrim(const string& str, const string& characters = " \t\r\n");
    string rtrim(const string& str, const string& characters = " \t\r\n");
    string trim(const string& str, const string& characters = " \t\r\n");

    string repeat(const string& str, int n = 1);

    string lpad(const string& str, int length, const string& padStr = " ");
    string rpad(const string& str, int length, const string& padStr = " ");
    string pad(const string& str, int length, const string& padStr = " ");

    string toLower(const string& str);
    string toUpper(const string& str);

    int indexOf(const string& str, const string& searchValue, int fromIndex = 0);
    int lastIndexOf(const string& str, const string& searchValue, int fromIndex = string::npos);

    string strLeft(const string& str, const string& sep);
    string strRight(const string& str, const string& sep);
    string strLeftBack(const string& str, const string& sep);
    string strRightBack(const string& str, const string& sep);

    bool includes(const string& str, const string& searchString, int position = 0);

    string truncate(const string& str, size_t length, const string& trunctateStr = "...");
    string surround(const string& str, const string& wrapper);
    string quote(const string& str, const char quoteChar = '"');
    string unquote(const string& str, const char quoteChar = '"');

    string replace(const string& str, const string& find, const string& replace, bool ignorecase = false);
    string replaceAll(const string& str, const string& find, const string& replace, bool ignorecase = false);
    // 根据模版替换，模板字段$xxx$
    // @example
    //   s.replaceTmpl("abc$UIN$def$ID$", R"({"UIN": "123", "ID": "4567"})"_json);
    //     => abc123def456
    string replaceTmpl(const string& str, json j);
    
    string reverse(const string& str);

    // Converts first letter of the string to uppercase. If true is passed as second argument the rest of the string will be converted to lower case.
    string capitalize(const string& str, bool lowercaseRest = false);
    // Converts first letter of the string to lowercase.
    string decapitalize(const string& str);
    // Converts underscored or dasherized string to a camelized one. Begins with a lower case letter unless it starts with an underscore, dash or an upper case letter.
    string camelize(const string& str, bool decap = false);
  };
  
  extern UnderscoreString &s;
}

#endif // !__UNDERSCORE_STRING_H__
