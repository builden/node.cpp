#include "core.h"
#include "iconv.h"

namespace nodepp {
  using std::ostringstream;

  string encodeURI(const string& str) {
    string src = iconv.strToUtf8(str);
    char hex[] = "0123456789ABCDEF";
    string dst;

    for (size_t i = 0; i < src.size(); ++i) {
      unsigned char cc = src[i];
      if (isascii(cc)) {
        if (cc == ' ') {
          dst += "%20";
        }
        else
          dst += cc;
      }
      else {
        unsigned char c = static_cast<unsigned char>(src[i]);
        dst += '%';
        dst += hex[c / 16];
        dst += hex[c % 16];
      }
    }
    return dst;
  }

  string decodeURI(const string& str) {
    string result;
    int hex = 0;
    for (size_t i = 0; i < str.length(); ++i) {
      switch (str[i]) {
      case '%':
        if (isxdigit(str[i + 1]) && isxdigit(str[i + 2]))
        {
          string hexStr = str.substr(i + 1, 2);
          hex = strtol(hexStr.c_str(), 0, 16);
          //字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
          //可以不经过编码直接用于URL
          if (!((hex >= 48 && hex <= 57) || //0-9
            (hex >= 97 && hex <= 122) ||   //a-z
            (hex >= 65 && hex <= 90) ||    //A-Z
            //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
            hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
            || hex == 0x2a || hex == 0x2b || hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
            || hex == 0x3A || hex == 0x3B || hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
            )) {
            result += char(hex);
            i += 2;
          }
          else result += '%';
        }
        else {
          result += '%';
        }
        break;
      default:
        result += str[i];
        break;
      }
    }
    return iconv.utf8ToStr(result);
  }

  string encodeURIComponent(const string& str) {
    string src = iconv.strToUtf8(str);
    char hex[] = "0123456789ABCDEF";
    char uriReserved[] = ";,/?:@&=+$# ";
    string dst;

    for (size_t i = 0; i < src.size(); ++i) {
      unsigned char cc = src[i];
      if (isascii(cc) && (strchr(uriReserved, cc) == nullptr)) {
        dst += cc;
      }
      else {
        unsigned char c = static_cast<unsigned char>(src[i]);
        dst += '%';
        dst += hex[c / 16];
        dst += hex[c % 16];
      }
    }
    return dst;
  }

  string decodeURIComponent(const string& str) {
    string result;
    int hex = 0;
    for (size_t i = 0; i < str.length(); ++i) {
      switch (str[i]) {
      case '%':
        if (isxdigit(str[i + 1]) && isxdigit(str[i + 2]))
        {
          string hexStr = str.substr(i + 1, 2);
          hex = strtol(hexStr.c_str(), 0, 16);
          //字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
          //可以不经过编码直接用于URL
          if (!((hex >= 48 && hex <= 57) || //0-9
            (hex >= 97 && hex <= 122) ||   //a-z
            (hex >= 65 && hex <= 90)    //A-Z
            )) {
            result += char(hex);
            i += 2;
          }
          else result += '%';
        }
        else {
          result += '%';
        }
        break;
      default:
        result += str[i];
        break;
      }
    }
    return iconv.utf8ToStr(result);
  }

  string escape(const string& str) {
    return "";
  }

  string unescape(const string& str) {
    return "";
  }

}