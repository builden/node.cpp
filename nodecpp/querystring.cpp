#include "querystring.h"
#include "core.h"
#include "underscore.string.h"
#include "lodash.h"
#include "fmt/format.h"
#include "iconv.h"
#include "error.h"

namespace nodecpp {
  uint32_t maxPairs = 1000;

  Querystring::Querystring() {
    for (int i = 0; i < 256; ++i) {
      hexTable.emplace_back(fmt::format("%{:02X}", i));
    }
  }

  json Querystring::parse(const string& querys, const string& sep /*= '&'*/, const string& eq /*= '='*/) {
    json obj = json({});

    auto sepLen = sep.length();
    auto eqLen = eq.length();
    bool customDecode = false;
    uint32_t pairs = maxPairs;

    svec_t keys;
    uint32_t lastPos = 0;
    uint32_t sepIdx = 0;
    uint32_t eqIdx = 0;
    string key = "";
    string value = "";
    bool keyEncoded = customDecode;
    bool valEncoded = customDecode;
    int encodeCheck = 0;
    for (size_t i = 0; i < querys.length(); ++i) {
      auto code = querys[i];

      // Try matching key/value pair separator (e.g. '&')
      if (code == sep[sepIdx]) {
        if (++sepIdx == sepLen) {
          // Key/value pair separator match!
          auto end = i - sepIdx + 1;
          if (eqIdx < eqLen) {
            // If we didn't find the key/value separator, treat the substring as
            // part of the key instead of the value
            if (lastPos < end)
              key += s.slice(querys, lastPos, end);
          }
          else if (lastPos < end)
            value += s.slice(querys, lastPos, end);
          if (keyEncoded)
            key = decodeStr(key);
          if (valEncoded)
            value = decodeStr(value);
          // Use a key array lookup instead of using hasOwnProperty(), which is
          // slower
          
          if (_.indexOf(keys, key) == -1) {
            obj[key] = value;
            keys.emplace_back(key);
          }
          else {
            auto curValue = obj[key];
            // `instanceof Array` is used instead of Array.isArray() because it
            // is ~15-20% faster with v8 4.7 and is safe to use because we are
            // using it with values being created within this function
            if (curValue.is_array()) {
              curValue.push_back(value);
            }
            else {
              obj[key] = json::array({curValue, value});
            }
          }
          if (--pairs == 0)
            break;
          keyEncoded = valEncoded = customDecode;
          encodeCheck = 0;
          key = value = "";
          lastPos = i + 1;
          sepIdx = eqIdx = 0;
        }
        continue;
      }
      else {
        sepIdx = 0;
        if (!valEncoded) {
          // Try to match an (valid) encoded byte (once) to minimize unnecessary
          // calls to string decoding functions
          if (code == 37/*%*/) {
            encodeCheck = 1;
          }
          else if (encodeCheck > 0 &&
            ((code >= 48/*0*/ && code <= 57/*9*/) ||
            (code >= 65/*A*/ && code <= 70/*F*/) ||
            (code >= 97/*a*/ && code <= 102/*f*/))) {
            if (++encodeCheck == 3)
              valEncoded = true;
          }
          else {
            encodeCheck = 0;
          }
        }
      }

      // Try matching key/value separator (e.g. '=') if we haven't already
      if (eqIdx < eqLen) {
        if (code == eq[eqIdx]) {
          if (++eqIdx == eqLen) {
            // Key/value separator match!
            auto end = i - eqIdx + 1;
            if (lastPos < end)
              key += s.slice(querys, lastPos, end);
            encodeCheck = 0;
            lastPos = i + 1;
          }
          continue;
        }
        else {
          eqIdx = 0;
          if (!keyEncoded) {
            // Try to match an (valid) encoded byte once to minimize unnecessary
            // calls to string decoding functions
            if (code == 37/*%*/) {
              encodeCheck = 1;
            }
            else if (encodeCheck > 0 &&
              ((code >= 48/*0*/ && code <= 57/*9*/) ||
              (code >= 65/*A*/ && code <= 70/*F*/) ||
              (code >= 97/*a*/ && code <= 102/*f*/))) {
              if (++encodeCheck == 3)
                keyEncoded = true;
            }
            else {
              encodeCheck = 0;
            }
          }
        }
      }

      if (code == 43/*+*/) {
        if (eqIdx < eqLen) {
          if (i - lastPos > 0)
            key += s.slice(querys, lastPos, i);
          key += "%20";
          keyEncoded = true;
        }
        else {
          if (i - lastPos > 0)
            value += s.slice(querys, lastPos, i);
          value += "%20";
          valEncoded = true;
        }
        lastPos = i + 1;
      }
    }

    // Check if we have leftover key or value data
    if (pairs > 0 && (lastPos < querys.length() || eqIdx > 0)) {
      if (lastPos < querys.length()) {
        if (eqIdx < eqLen)
          key += s.slice(querys, lastPos);
        else if (sepIdx < sepLen)
          value += s.slice(querys, lastPos);
      }
      if (keyEncoded)
        key = decodeStr(key);
      if (valEncoded)
        value = decodeStr(value);
      // Use a key array lookup instead of using hasOwnProperty(), which is
      // slower

      if (_.indexOf(keys, key) == -1) {
        obj[key] = value;
        keys.emplace_back(key);
      }
      else {
        auto curValue = obj[key];
        // `instanceof Array` is used instead of Array.isArray() because it
        // is ~15-20% faster with v8 4.7 and is safe to use because we are
        // using it with values being created within this function
        if (curValue.is_array()) {
          curValue.push_back(value);
        }
        else {
          obj[key] = json::array({curValue, value});
        }
      }
    }

    return obj;
  }

  string Querystring::stringify(json& obj, const string& sep /*= "&"*/, const string& eq/*= "="*/) {
    string fields = "";
    size_t len = obj.size();
    size_t flast = len - 1;
    size_t i = 0;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
      auto k = it.key();
      auto v = it.value();
      auto ks = encode(k) + eq;

      if (v.is_array()) {
        auto vlen = v.size();
        auto vlast = vlen - 1;
        for (size_t j = 0; j < vlen; ++j) {
          fields += ks + encode(stringifyPrimitive(v[j]));
          if (j < vlast) fields += sep;
        }
        if (vlen && i < flast) fields += sep;
      }
      else {
        fields += ks + encode(stringifyPrimitive(v));
        if (i < flast) fields += sep;
      }
      i++;
    }
    return fields;
  }

  string Querystring::unescape(const string& str) {
    return decodeURIComponent(str);
  }

  string Querystring::decodeStr(const string& str) {
    return unescape(str);
  }

  string Querystring::escape(const string& str) {
    // replaces encodeURIComponent
    // http://www.ecma-international.org/ecma-262/5.1/#sec-15.1.3.4
    string out = "";
    size_t lastPos = 0;
    wstring wstr = iconv.stow(str);

    for (size_t i = 0; i < wstr.length(); ++i) {
      uint32_t c = iconv.charCodeAt(wstr, i);

      // These characters do not need escaping (in order):
      // ! - . _ ~
      // ' ( ) *
      // digits
      // alpha (uppercase)
      // alpha (lowercase)
      if (c == 0x21 || c == 0x2D || c == 0x2E || c == 0x5F || c == 0x7E ||
        (c >= 0x27 && c <= 0x2A) ||
        (c >= 0x30 && c <= 0x39) ||
        (c >= 0x41 && c <= 0x5A) ||
        (c >= 0x61 && c <= 0x7A)) {
        continue;
      }

      if (i - lastPos > 0)
        out += s.slice(str, lastPos, i);

      // Other ASCII characters
      if (c < 0x80) {
        lastPos = i + 1;
        out += hexTable[c];
        continue;
      }

      // Multi-byte characters ...
      if (c < 0x800) {
        lastPos = i + 1;
        out += hexTable[0xC0 | (c >> 6)] + hexTable[0x80 | (c & 0x3F)];
        continue;
      }
      if (c < 0xD800 || c >= 0xE000) {
        lastPos = i + 1;
        out += hexTable[0xE0 | (c >> 12)] +
          hexTable[0x80 | ((c >> 6) & 0x3F)] +
          hexTable[0x80 | (c & 0x3F)];
        continue;
      }
      // Surrogate pair
      ++i;
      uint32_t c2 = 0;
      if (i < wstr.length())
        c2 = iconv.charCodeAt(wstr, i) & 0x3FF;
      else
        throw Error(-2001, "URI malformed");
      lastPos = i + 1;
      c = 0x10000 + (((c & 0x3FF) << 10) | c2);
      out += hexTable[0xF0 | (c >> 18)] +
        hexTable[0x80 | ((c >> 12) & 0x3F)] +
        hexTable[0x80 | ((c >> 6) & 0x3F)] +
        hexTable[0x80 | (c & 0x3F)];
    }
    if (lastPos == 0)
      return str;
    if (lastPos < wstr.length())
      return out + iconv.wtos(s.slice(wstr, lastPos));
    return out;
  }

  string Querystring::stringifyPrimitive(json& obj) {
    if (obj.is_string()) {
      return obj.get<string>();
    }
    if (obj.is_number_float()) {
      return fmt::format("{}", obj.get<double>());
    }
    if (obj.is_number_integer()) {
      return fmt::format("{}", obj.get<int64_t>());
    }
    if (obj.is_number_unsigned()) {
      return fmt::format("{}", obj.get <uint64_t>());
    }
    if (obj.is_boolean()) {
      return fmt::format("{}", obj.get<bool>());
    }
    return "";
  }

  string Querystring::encode(const string& str) {
    return escape(str);
  }

  Querystring &qs = Querystring::instance();
}