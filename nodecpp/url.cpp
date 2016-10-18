#include "url.h"
#include "core.h"
#include "iconv.h"
#include "lodash.h"
#include "fmt/format.h"
#include "underscore.string.h"
#include <regex>

namespace nodecpp {
  // define these here so at least they only have to be
  // compiled once on the first module load.
  std::regex protocolPattern("^([a-z0-9.+-]+:)", std::regex_constants::icase);
  std::regex portPattern(":[0-9]*$");

  svec_t unsafeProtocol = {
    "javascript",
    "javascript:"
  };

  svec_t hostlessProtocol = {
    "javascript",
    "javascript:"
  };
  svec_t slashedProtocol = {
    "http",
    "http:",
    "https",
    "https:",
    "ftp",
    "ftp:",
    "gopher",
    "gopher:",
    "file",
    "file:"
  };

  uint32_t hostnameMaxLen = 255;

  // Special case for a simple path URL
  std::regex simplePathPattern(R"(^(\/\/?(?!\/)[^\?\s]*)(\?[^\s]*)?$)");

  Url::Url() {
    hexTable.resize(256);
    for (int i = 0; i < 256; ++i) {
      hexTable[i] = "%" + fmt::format("{:02X}", i);
    }
  }

  string Url::format(const UrlObj& /*urlObj*/) {
    return "";
  }

  UrlObj Url::parse(const string& urlStr, bool slashesDenoteHost /*= false*/) {
    UrlObj urlObj;
    // Copy chrome, IE, opera backslash-handling behavior.
    // Back slashes before the query string get converted to forward slashes
    // See: https://code.google.com/p/chromium/issues/detail?id=25916
    bool hasHash = false;
    auto start = -1;
    auto end = -1;
    string rest = "";
    int lastPos = 0;
    uint32_t i = 0;
    for (bool inWs = false, split = false; i < urlStr.length(); ++i) {
      auto code = iconv.charCodeAt(urlStr, i);

      // Find first and last non-whitespace characters for trimming
      bool isWs = code == 32/* */ ||
        code == 9/*\t*/ ||
        code == 13/*\r*/ ||
        code == 10/*\n*/ ||
        code == 12/*\f*/ ||
        code == 160/*\u00A0*/ ||
        code == 65279/*\uFEFF*/;
      if (start == -1) {
        if (isWs)
          continue;
        lastPos = start = i;
      }
      else {
        if (inWs) {
          if (!isWs) {
            end = -1;
            inWs = false;
          }
        }
        else if (isWs) {
          end = i;
          inWs = true;
        }
      }

      // Only convert backslashes while we haven't seen a split character
      if (!split) {
        switch (code) {
        case 35: // '#'
          hasHash = true;
          // Fall through
        case 63: // '?'
          split = true;
          break;
        case 92: // '\\'
          if (i - lastPos > 0)
            rest += s.slice(urlStr, lastPos, i);
          rest += '/';
          lastPos = i + 1;
          break;
        }
      }
      else if (!hasHash && code == 35/*#*/) {
        hasHash = true;
      }
    }

    // Check if string was non-empty (including strings with only whitespace)
    if (start != -1) {
      if (lastPos == start) {
        // We didn't convert any backslashes

        if (end == -1) {
          if (start == 0)
            rest = urlStr;
          else
            rest = s.slice(urlStr, start);
        }
        else {
          rest = s.slice(urlStr, start, end);
        }
      }
      else if (end == -1 && lastPos < static_cast<int>(urlStr.length())) {
        // We converted some backslashes and have only part of the entire string
        rest += s.slice(urlStr, lastPos);
      }
      else if (end != -1 && lastPos < end) {
        // We converted some backslashes and have only part of the entire string
        rest += s.slice(urlStr, lastPos, end);
      }
    }

    if (!slashesDenoteHost && !hasHash) {
      // Try fast path regexp
      std::smatch simplePath;
      std::regex_search(rest, simplePath, simplePathPattern);
      if (!simplePath.empty()) {
        urlObj.path = rest;
        urlObj.href = rest;
        urlObj.pathname = simplePath.str(1);
        if (!simplePath.str(2).empty()) {
          urlObj.search = simplePath.str(2);
          urlObj.query = s.slice(urlObj.search, 1);
        }
        return urlObj;
      }
    }
    std::smatch protoMatch;
    std::regex_search(rest, protoMatch, protocolPattern);
    string proto;
    string lowerProto;
    if (!protoMatch.empty()) {
      proto = protoMatch.str(0);
      lowerProto = s.toLower(proto);
      urlObj.protocol = lowerProto;
      rest = s.slice(rest, proto.length());
    }

    // figure out if it's got a host
    // user@server is *always* interpreted as a hostname, and url
    // resolution will treat //foo/bar as host=foo,path=bar because that's
    // how the browser resolves relative URLs.
    bool slashes = false;
    if (slashesDenoteHost || !proto.empty() || std::regex_match(rest, std::regex(R"(^\/\/[^@\/]+@[^@\/]+)"))) {
      slashes = rest[0] == 47/*/*/ &&
        rest[1] == 47/*/*/;
      if (slashes && !(!proto.empty() && _.includes(hostlessProtocol, proto))) {
        rest = s.slice(rest, 2);
        urlObj.slashes = true;
      }
    }

    if (!_.includes(hostlessProtocol, proto) &&
      (slashes || (!proto.empty() && _.includes(slashedProtocol, proto)))) {

      // there's a hostname.
      // the first instance of /, ?, ;, or # ends the host.
      //
      // If there is an @ in the hostname, then non-host chars *are* allowed
      // to the left of the last @ sign, unless some host-ending character
      // comes *before* the @-sign.
      // URLs are obnoxious.
      //
      // ex:
      // http://a@b@c/ => user:a@b host:c
      // http://a@b?@c => user:a host:b path:/?@c

      // v0.12 TODO(isaacs): This is not quite how Chrome does things.
      // Review our test case against browsers more comprehensively.

      int hostEnd = -1;
      int atSign = -1;
      int nonHost = -1;
      for (i = 0; i < rest.length(); ++i) {
        switch (rest[i]) {
        case 9:   // '\t'
        case 10:  // '\n'
        case 13:  // '\r'
        case 32:  // ' '
        case 34:  // '"'
        case 37:  // '%'
        case 39:  // '\''
        case 59:  // ';'
        case 60:  // '<'
        case 62:  // '>'
        case 92:  // '\\'
        case 94:  // '^'
        case 96:  // '`'
        case 123: // '{'
        case 124: // '|'
        case 125: // '}'
          // Characters that are never ever allowed in a hostname from RFC 2396
          if (nonHost == -1)
            nonHost = i;
          break;
        case 35: // '#'
        case 47: // '/'
        case 63: // '?'
          // Find the first instance of any host-ending characters
          if (nonHost == -1)
            nonHost = i;
          hostEnd = i;
          break;
        case 64: // '@'
          // At this point, either we have an explicit point where the
          // auth portion cannot go past, or the last @ char is the decider.
          atSign = i;
          nonHost = -1;
          break;
        }
        if (hostEnd != -1)
          break;
      }
      start = 0;
      if (atSign != -1) {
        urlObj.auth = decodeURIComponent(s.slice(rest, 0, atSign));
        start = atSign + 1;
      }
      if (nonHost == -1) {
        urlObj.host = s.slice(rest, start);
        rest = "";
      }
      else {
        urlObj.host = s.slice(rest, start, nonHost);
        rest = s.slice(rest, nonHost);
      }

      // pull out port.
      parseHost(urlObj);

      string hostname = urlObj.hostname;

      // if hostname begins with [ and ends with ]
      // assume that it's an IPv6 address.
      bool ipv6Hostname = hostname[0] == 91/*[*/ &&
        hostname[hostname.length() - 1] == 93/*]*/;

      // validate a little.
      if (!ipv6Hostname) {
        string result = validateHostname(urlObj, rest, hostname);
        if (!result.empty()) rest = result;
      }

      if (urlObj.hostname.length() > hostnameMaxLen) {
        urlObj.hostname = "";
      }
      else {
        // hostnames are always lower case.
        urlObj.hostname = s.toLower(urlObj.hostname);
      }
// 
//       if (!ipv6Hostname) {
//         // IDNA Support: Returns a punycoded representation of "domain".
//         // It only converts parts of the domain name that
//         // have non-ASCII characters, i.e. it doesn't matter if
//         // you call it with a domain that already is ASCII-only.
//         this.hostname = punycode.toASCII(this.hostname);
//       }

      string p = !urlObj.port.empty() ? fmt::format(":{}", urlObj.port) : "";
      urlObj.host = urlObj.hostname + p;

      // strip [ and ] from the hostname
      // the host field still retains them, though
      if (ipv6Hostname) {
        urlObj.hostname = s.slice(urlObj.hostname, 1, -1);
        if (rest[0] != '/') {
          rest = "/" + rest;
        }
      }
    }

    // now rest is set to the post-host stuff.
    // chop off any delim chars.
    if (!_.includes(unsafeProtocol, lowerProto)) {
      // First, make 100% sure that any "autoEscape" chars get
      // escaped, even if encodeURIComponent doesn't think they
      // need to be.
      string result = autoEscapeStr(rest);
      if (!result.empty()) rest = result;
    }

    int questionIdx = -1;
    int hashIdx = -1;
    for (i = 0; i < rest.length(); ++i) {
      uint32_t code = rest[i];
      if (code == 35/*#*/) {
        urlObj.hash = s.slice(rest, i);
        hashIdx = i;
        break;
      }
      else if (code == 63/*?*/ && questionIdx == -1) {
        questionIdx = i;
      }
    }

    if (questionIdx != -1) {
      if (hashIdx == -1) {
        urlObj.search = s.slice(rest, questionIdx);
        urlObj.query = s.slice(rest, questionIdx + 1);
      }
      else {
        urlObj.search = s.slice(rest, questionIdx, hashIdx);
        urlObj.query = s.slice(rest, questionIdx + 1, hashIdx);
      }
    }

    int firstIdx = (questionIdx != -1 &&
      (hashIdx == -1 || questionIdx < hashIdx)
      ? questionIdx
      : hashIdx);
    if (firstIdx == -1) {
      if (rest.length() > 0)
        urlObj.pathname = rest;
    }
    else if (firstIdx > 0) {
      urlObj.pathname = s.slice(rest, 0, firstIdx);
    }
    if (_.includes(slashedProtocol, lowerProto) &&
      !urlObj.hostname.empty() && urlObj.pathname.empty()) {
      urlObj.pathname = "/";
    }

    urlObj.path = urlObj.pathname + urlObj.search;

    // finally, reconstruct the href based on what has been validated.
    urlObj.href = formatHref(urlObj);
    return urlObj;
  }

  string Url::resolve(const string& /*from*/, const string& /*to*/) {
    return "";
  }

  void Url::parseHost(UrlObj& obj) {
    string host = obj.host;
    std::smatch portMatch;
    std::regex_search(obj.host, portMatch, portPattern);
    if (!portMatch.empty()) {
      string port = portMatch.str(0);
      if (port != ":") {
        obj.port = s.slice(port, 1);
      }
      host = s.slice(host, 0, host.length() - port.length());
    }
    if (!host.empty()) obj.hostname = host;
  }

  string Url::validateHostname(UrlObj& obj, const string& rest, const string& hostname) {
    for (size_t i = 0, lastPos = 0; i <= hostname.length(); ++i) {
      uint32_t code = 0;
      if (i < hostname.length())
        code = hostname[i];
      if (code == 46/*.*/ || i == hostname.length()) {
        if (i - lastPos > 0) {
          if (i - lastPos > 63) {
            obj.hostname = s.slice(hostname, 0, lastPos + 63);
            return "/" + s.slice(hostname, lastPos + 63) + rest;
          }
        }
        lastPos = i + 1;
        continue;
      }
      else if ((code >= 48/*0*/ && code <= 57/*9*/) ||
        (code >= 97/*a*/ && code <= 122/*z*/) ||
        code == 45/*-*/ ||
        (code >= 65/*A*/ && code <= 90/*Z*/) ||
        code == 43/*+*/ ||
        code == 95/*_*/ ||
        code > 127) {
        continue;
      }
      // Invalid host character
      obj.hostname = s.slice(hostname, 0, i);
      if (i < hostname.length())
        return "/" + s.slice(hostname, i) + rest;
      break;
    }
    return "";
  }

  string Url::autoEscapeStr(const string& rest) {
    string newRest = "";
    uint32_t lastPos = 0;
    for (uint32_t i = 0; i < rest.length(); ++i) {
      // Automatically escape all delimiters and unwise characters from RFC 2396
      // Also escape single quotes in case of an XSS attack
      switch (rest[i]) {
      case 9:   // '\t'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%09";
        lastPos = i + 1;
        break;
      case 10:  // '\n'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%0A";
        lastPos = i + 1;
        break;
      case 13:  // '\r'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%0D";
        lastPos = i + 1;
        break;
      case 32:  // ' '
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%20";
        lastPos = i + 1;
        break;
      case 34:  // '"'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%22";
        lastPos = i + 1;
        break;
      case 39:  // '\''
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%27";
        lastPos = i + 1;
        break;
      case 60:  // '<'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%3C";
        lastPos = i + 1;
        break;
      case 62:  // '>'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%3E";
        lastPos = i + 1;
        break;
      case 92:  // '\\'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%5C";
        lastPos = i + 1;
        break;
      case 94:  // '^'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%5E";
        lastPos = i + 1;
        break;
      case 96:  // '`'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%60";
        lastPos = i + 1;
        break;
      case 123: // '{'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%7B";
        lastPos = i + 1;
        break;
      case 124: // '|'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%7C";
        lastPos = i + 1;
        break;
      case 125: // '}'
        if (i - lastPos > 0)
          newRest += s.slice(rest, lastPos, i);
        newRest += "%7D";
        lastPos = i + 1;
        break;
      }
    }
    if (lastPos == 0)
      return newRest;
    if (lastPos < rest.length())
      return newRest + s.slice(rest, lastPos);
    else
      return newRest;
  }

  string Url::formatHref(UrlObj& obj) {
    string auth = obj.auth;
    if (!auth.empty()) {
      auth = encodeAuth(auth);
      auth += "@";
    }

    string protocol = obj.protocol;
    string pathname = obj.pathname;
    string hash = obj.hash;
    string host;
    string query = "";

    if (!obj.host.empty()) {
      host = auth + obj.host;
    }
    else if (!obj.hostname.empty()) {
      host = auth + (s.indexOf(obj.hostname, ":") == -1 ?
        obj.hostname :
        "[" + obj.hostname + "]");
      if (!obj.port.empty()) {
        host += ":" + obj.port;
      }
    }

    string search = obj.search;
    if (search.empty() && !query.empty()) search = "?" + query;

    if (!protocol.empty() && iconv.charCodeAt(protocol, protocol.length() - 1) != 58/*:*/)
      protocol += ":";

    string newPathname = "";
    uint32_t lastPos = 0;
    for (uint32_t i = 0; i < pathname.length(); ++i) {
      switch (pathname[i]) {
      case 35: // '#'
        if (i - lastPos > 0)
          newPathname += s.slice(pathname, lastPos, i);
        newPathname += "%23";
        lastPos = i + 1;
        break;
      case 63: // '?'
        if (i - lastPos > 0)
          newPathname += s.slice(pathname, lastPos, i);
        newPathname += "%3F";
        lastPos = i + 1;
        break;
      }
    }
    if (lastPos > 0) {
      if (lastPos != pathname.length())
        pathname = newPathname + s.slice(pathname, lastPos);
      else
        pathname = newPathname;
    }

    // only the slashedProtocols get the //.  Not mailto:, xmpp:, etc.
    // unless they had them to begin with.
    if (obj.slashes ||
      (protocol.empty() || _.includes(slashedProtocol, protocol)) && !host.empty()) {
      host = "//" + host;
      if (!pathname.empty() && pathname[0] != 47/*/*/)
        pathname = "/" + pathname;
    }
    else if (host.empty()) {
      host = "";
    }

    search = s.replace(search, "#", "%23");

    if (!hash.empty() && hash[0] != 35/*#*/) hash = "#" + hash;
    if (!search.empty() && search[0] != 63/*?*/) search = "?" + search;

    return protocol + host + pathname + search + hash;
  }

  string Url::encodeAuth(const string& str) {
    // faster encodeURIComponent alternative for encoding auth uri components
    string out = "";
    uint32_t lastPos = 0;
    wstring wstr = iconv.stow(str);
    uint32_t strLen = wstr.length();
    for (uint32_t i = 0; i < strLen; ++i) {
      uint32_t c = iconv.charCodeAt(wstr, i);

      // These characters do not need escaping:
      // ! - . _ ~
      // ' ( ) * :
      // digits
      // alpha (uppercase)
      // alpha (lowercase)
      if (c == 0x21 || c == 0x2D || c == 0x2E || c == 0x5F || c == 0x7E ||
        (c >= 0x27 && c <= 0x2A) ||
        (c >= 0x30 && c <= 0x3A) ||
        (c >= 0x41 && c <= 0x5A) ||
        (c >= 0x61 && c <= 0x7A)) {
        continue;
      }

      if (i - lastPos > 0)
        out += s.slice(str, lastPos, i);

      lastPos = i + 1;

      // Other ASCII characters
      if (c < 0x80) {
        out += hexTable[c];
        continue;
      }

      // Multi-byte characters ...
      if (c < 0x800) {
        out += hexTable[0xC0 | (c >> 6)] + hexTable[0x80 | (c & 0x3F)];
        continue;
      }
      if (c < 0xD800 || c >= 0xE000) {
        out += hexTable[0xE0 | (c >> 12)] +
          hexTable[0x80 | ((c >> 6) & 0x3F)] +
          hexTable[0x80 | (c & 0x3F)];
        continue;
      }
      // Surrogate pair
      ++i;
      uint32_t c2;
      if (i < strLen)
        c2 = iconv.charCodeAt(wstr, i) & 0x3FF;
      else
        c2 = 0;
      c = 0x10000 + (((c & 0x3FF) << 10) | c2);
      out += hexTable[0xF0 | (c >> 18)] +
        hexTable[0x80 | ((c >> 12) & 0x3F)] +
        hexTable[0x80 | ((c >> 6) & 0x3F)] +
        hexTable[0x80 | (c & 0x3F)];
    }
    if (lastPos == 0)
      return str;
    if (lastPos < strLen)
      return out + s.slice(str, lastPos);
    return out;
  }

  Url &url = Url::instance();

}