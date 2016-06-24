#include "path.h"
#include "underscore.string.h"
#include "process.h"
#include "std-regex-ex.h"
#include "fmt/format.h"

namespace nodepp {

  string Path::basename(const string& path, const string& ext /*= ""*/) {
    auto start = 0;
    auto end = -1;
    auto matchedSlash = true;
    int i = 0;

    // Check for a drive letter prefix so as not to mistake the following
    // path separator as an extra separator at the end of the path that can be
    // disregarded
    if (path.length() >= 2) {
      auto drive = path[0];
      if ((drive >= 65/*A*/ && drive <= 90/*Z*/) ||
        (drive >= 97/*a*/ && drive <= 122/*z*/)) {
        if (path[1] == 58/*:*/)
          start = 2;
      }
    }

    if (ext.length() > 0 && ext.length() <= path.length()) {
      if (ext.length() == path.length() && ext == path)
        return "";
      int extIdx = ext.length() - 1;
      auto firstNonSlashEnd = -1;
      for (i = path.length() - 1; i >= start; --i) {
        auto code = path[i];
        if (code == 47/*/*/ || code == 92/*\*/) {
          // If we reached a path separator that was not part of a set of path
          // separators at the end of the string, stop now
          if (!matchedSlash) {
            start = i + 1;
            break;
          }
        }
        else {
          if (firstNonSlashEnd == -1) {
            // We saw the first non-path separator, remember this index in case
            // we need it if the extension ends up not matching
            matchedSlash = false;
            firstNonSlashEnd = i + 1;
          }
          if (extIdx >= 0) {
            // Try to match the explicit extension
            if (code == ext[extIdx]) {
              if (--extIdx == -1) {
                // We matched the extension, so mark this as the end of our path
                // component
                end = i;
              }
            }
            else {
              // Extension does not match, so our result is the entire path
              // component
              extIdx = -1;
              end = firstNonSlashEnd;
            }
          }
        }
      }

      if (end == -1)
        return "";
      return s.slice(path, start, end);
    }
    else {
      for (i = path.length() - 1; i >= start; --i) {
        auto code = path[i];
        if (code == 47/*/*/ || code == 92/*\*/) {
          // If we reached a path separator that was not part of a set of path
          // separators at the end of the string, stop now
          if (!matchedSlash) {
            start = i + 1;
            break;
          }
        }
        else if (end == -1) {
          // We saw the first non-path separator, mark this as the end of our
          // path component
          matchedSlash = false;
          end = i + 1;
        }
      }

      if (end == -1)
        return "";
      return s.slice(path, start, end);
    }
  }

  string Path::dirname(const string& path) {
    int len = path.length();
    if (len == 0)
      return ".";
    auto rootEnd = -1;
    auto end = -1;
    auto matchedSlash = true;
    auto offset = 0;
    auto code = path[0];

    // Try to match a root
    if (len > 1) {
      if (code == 47/*/*/ || code == 92/*\*/) {
        // Possible UNC root

        rootEnd = offset = 1;

        code = path[1];
        if (code == 47/*/*/ || code == 92/*\*/) {
          // Matched double path separator at beginning
          auto j = 2;
          auto last = j;
          // Match 1 or more non-path separators
          for (; j < len; ++j) {
            code = path[j];
            if (code == 47/*/*/ || code == 92/*\*/)
              break;
          }
          if (j < len && j != last) {
            // Matched!
            last = j;
            // Match 1 or more path separators
            for (; j < len; ++j) {
              code = path[j];
              if (code != 47/*/*/ && code != 92/*\*/)
                break;
            }
            if (j < len && j != last) {
              // Matched!
              last = j;
              // Match 1 or more non-path separators
              for (; j < len; ++j) {
                code = path[j];
                if (code == 47/*/*/ || code == 92/*\*/)
                  break;
              }
              if (j == len) {
                // We matched a UNC root only
                return path;
              }
              if (j != last) {
                // We matched a UNC root with leftovers

                // Offset by 1 to include the separator after the UNC root to
                // treat it as a "normal root" on top of a (UNC) root
                rootEnd = offset = j + 1;
              }
            }
          }
        }
      }
      else if ((code >= 65/*A*/ && code <= 90/*Z*/) ||
        (code >= 97/*a*/ && code <= 122/*z*/)) {
        // Possible device root

        code = path[1];
        if (path[1] == 58/*:*/) {
          rootEnd = offset = 2;
          if (len > 2) {
            code = path[2];
            if (code == 47/*/*/ || code == 92/*\*/)
              rootEnd = offset = 3;
          }
        }
      }
    }
    else if (code == 47/*/*/ || code == 92/*\*/) {
      return "" + path[0];
    }

    for (auto i = len - 1; i >= offset; --i) {
      code = path[i];
      if (code == 47/*/*/ || code == 92/*\*/) {
        if (!matchedSlash) {
          end = i;
          break;
        }
      }
      else  {
        // We saw the first non-path separator
        matchedSlash = false;
      }
    }

    if (end == -1) {
      if (rootEnd == -1)
        return ".";
      else
        end = rootEnd;
    }
    return s.slice(path, 0, end);
  }

  string Path::extname(const string& path) {
    auto startDot = -1;
    auto startPart = 0;
    auto end = -1;
    auto matchedSlash = true;
    // Track the state of characters (if any) we see before our first dot and
    // after any path separator we find
    auto preDotState = 0;
    for (auto i = path.length() - 1; i >= 0; --i) {
      auto code = path[i];
      if (code == 47/*/*/ || code == 92/*\*/) {
        // If we reached a path separator that was not part of a set of path
        // separators at the end of the string, stop now
        if (!matchedSlash) {
          startPart = i + 1;
          break;
        }
        continue;
      }
      if (end == -1) {
        // We saw the first non-path separator, mark this as the end of our
        // extension
        matchedSlash = false;
        end = i + 1;
      }
      if (code == 46/*.*/) {
        // If this is our first dot, mark it as the start of our extension
        if (startDot == -1)
          startDot = i;
        else if (preDotState != 1)
          preDotState = 1;
      }
      else if (startDot != -1) {
        // We saw a non-dot and non-path separator before our dot, so we should
        // have a good chance at having a non-empty extension
        preDotState = -1;
      }
    }

    if (startDot == -1 ||
      end == -1 ||
      // We saw a non-dot character immediately before the dot
      preDotState == 0 ||
      // The (right-most) trimmed path component is exactly '..'
      (preDotState == 1 &&
      startDot == end - 1 &&
      startDot == startPart + 1)) {
      return "";
    }
    return s.slice(path, startDot, end);
  }

  string Path::join(const string& path1
    , const string& path2
    , const string& path3
    , const string& path4
    , const string& path5) {
    string joined;
    string firstPart;
    std::vector<string> pathes{ path1, path2, path3, path4, path5 };
    for (size_t i = 0; i < pathes.size(); ++i) {
      auto arg = pathes[i];
      if (arg.length() > 0) {
        if (joined.empty())
          joined = firstPart = arg;
        else
          joined += '\\' + arg;
      }
    }

    if (joined.empty())
      return ".";

    // Make sure that the joined path doesn't start with two slashes, because
    // normalize() will mistake it for an UNC path then.
    //
    // This step is skipped when it is very clear that the user actually
    // intended to point at an UNC path. This is assumed when the first
    // non-empty string arguments starts with exactly two slashes followed by
    // at least one more non-slash character.
    //
    // Note that for normalize() to treat a path as an UNC path it needs to
    // have at least 2 components, so we don't filter for that here.
    // This means that the user can use join to construct UNC paths from
    // a server name and a share name; for example:
    //   path.join('//server', 'share') -> '\\\\server\\share\\')
    //var firstPart = paths[0];
    auto needsReplace = true;
    size_t slashCount = 0;
    auto code = firstPart[0];
    if (code == 47/*/*/ || code == 92/*\*/) {
      ++slashCount;
      auto firstLen = firstPart.length();
      if (firstLen > 1) {
        code = firstPart[1];
        if (code == 47/*/*/ || code == 92/*\*/) {
          ++slashCount;
          if (firstLen > 2) {
            code = firstPart[2];
            if (code == 47/*/*/ || code == 92/*\*/)
              ++slashCount;
            else {
              // We matched a UNC path in the first part
              needsReplace = false;
            }
          }
        }
      }
    }
    if (needsReplace) {
      // Find any more consecutive slashes we need to replace
      for (; slashCount < joined.length(); ++slashCount) {
        code = joined[slashCount];
        if (code != 47/*/*/ && code != 92/*\*/)
          break;
      }

      // Replace the slashes if needed
      if (slashCount >= 2)
        joined = "\\" + s.slice(joined, slashCount);
    }

    return normalize(joined);
  }

  string Path::normalize(const string& path) {
    auto len = path.length();
    if (len == 0)
      return ".";
    size_t rootEnd = 0;
    auto code = path[0];
    string device;
    bool isAbsolute = false;

    // Try to match a root
    if (len > 1) {
      if (code == 47/*/*/ || code == 92/*\*/) {
        // Possible UNC root

        // If we started with a separator, we know we at least have an absolute
        // path of some kind (UNC or otherwise)
        isAbsolute = true;

        code = path[1];
        if (code == 47/*/*/ || code == 92/*\*/) {
          // Matched double path separator at beginning
          size_t j = 2;
          auto last = j;
          // Match 1 or more non-path separators
          for (; j < len; ++j) {
            code = path[j];
            if (code == 47/*/*/ || code == 92/*\*/)
              break;
          }
          if (j < len && j != last) {
            string firstPart = s.slice(path, last, j);
            // Matched!
            last = j;
            // Match 1 or more path separators
            for (; j < len; ++j) {
              code = path[j];
              if (code != 47/*/*/ && code != 92/*\*/)
                break;
            }
            if (j < len && j != last) {
              // Matched!
              last = j;
              // Match 1 or more non-path separators
              for (; j < len; ++j) {
                code = path[j];
                if (code == 47/*/*/ || code == 92/*\*/)
                  break;
              }
              if (j == len) {
                // We matched a UNC root only
                // Return the normalized version of the UNC root since there
                // is nothing left to process

                return "\\\\" + firstPart + "\\" + s.slice(path, last) + "\\";
              }
              else if (j != last) {
                // We matched a UNC root with leftovers

                device = "\\\\" + firstPart + "\\" + s.slice(path, last, j);
                rootEnd = j;
              }
            }
          }
        }
        else {
          rootEnd = 1;
        }
      }
      else if ((code >= 65/*A*/ && code <= 90/*Z*/) ||
        (code >= 97/*a*/ && code <= 122/*z*/)) {
        // Possible device root

        code = path[1];
        if (path[1] == 58/*:*/) {
          device = s.slice(path, 0, 2);
          rootEnd = 2;
          if (len > 2) {
            code = path[2];
            if (code == 47/*/*/ || code == 92/*\*/) {
              // Treat separator following drive name as an absolute path
              // indicator
              isAbsolute = true;
              rootEnd = 3;
            }
          }
        }
      }
    }
    else if (code == 47/*/*/ || code == 92/*\*/) {
      // `path` contains just a path separator, exit early to avoid unnecessary
      // work
      return "\\";
    }

    code = path[len - 1];
    auto trailingSeparator = (code == 47/*/*/ || code == 92/*\*/);
    string tail;
    if (rootEnd < len)
      tail = normalizeStringWin32(s.slice(path, rootEnd), !isAbsolute);
    else
      tail = "";
    if (tail.length() == 0 && !isAbsolute)
      tail = '.';
    if (tail.length() > 0 && trailingSeparator)
      tail += '\\';
    if (device.empty()) {
      if (isAbsolute) {
        if (tail.length() > 0)
          return "\\" + tail;
        else
          return "\\";
      }
      else if (tail.length() > 0) {
        return tail;
      }
      else {
        return "";
      }
    }
    else {
      if (isAbsolute) {
        if (tail.length() > 0)
          return device + '\\' + tail;
        else
          return device + '\\';
      }
      else if (tail.length() > 0) {
        return device + tail;
      }
      else {
        return device;
      }
    }
  }

  bool Path::isAbsolute(const string& path) {
    auto len = path.length();
    if (len == 0) return false;
    auto code = path[0];
    if (code == 47/*/*/ || code == 92/*\*/) {
      return true;
    }
    else if ((code >= 65/*A*/ && code <= 90/*Z*/) ||
      (code >= 97/*a*/ && code <= 122/*z*/)) {
      // Possible device root

      if (len > 2 && path[1] == 58/*:*/) {
        code = path[2];
        if (code == 47/*/*/ || code == 92/*\*/)
          return true;
      }
    }
    return false;
  }

  string Path::normalizeStringWin32(const string& path, bool allowAboveRoot) {
    string res = "";
    auto lastSlash = -1;
    auto dots = 0;
    auto code = 0;
    for (size_t i = 0; i <= path.length(); ++i) {
      if (i < path.length())
        code = path[i];
      else if (code == 47/*/*/ || code == 92/*\*/)
        break;
      else
        code = 47/*/*/;
      if (code == 47/*/*/ || code == 92/*\*/) {
        if (lastSlash == i - 1 || dots == 1) {
          // NOOP
        }
        else if (lastSlash != i - 1 && dots == 2) {
          if (res.length() < 2 ||
            res[(res.length() - 1)] != 46/*.*/ ||
            res[res.length() - 2] != 46/*.*/) {
            if (res.length() > 2) {
              auto start = res.length() - 1;
              auto j = start;
              for (; j >= 0; --j) {
                if (res[j] == 92/*\*/)
                  break;
              }
              if (j != start) {
                if (j == -1)
                  res = "";
                else
                  res = s.slice(res, 0, j);
                lastSlash = i;
                dots = 0;
                continue;
              }
            }
            else if (res.length() == 2 || res.length() == 1) {
              res = "";
              lastSlash = i;
              dots = 0;
              continue;
            }
          }
          if (allowAboveRoot) {
            if (res.length() > 0)
              res += "\\..";
            else
              res = "..";
          }
        }
        else {
          if (res.length() > 0)
            res += '\\' + s.slice(path, lastSlash + 1, i);
          else
            res = s.slice(path, lastSlash + 1, i);
        }
        lastSlash = i;
        dots = 0;
      }
      else if (code == 46/*.*/ && dots != -1) {
        ++dots;
      }
      else {
        dots = -1;
      }
    }
    return res;
  }

  string Path::resolve(const string& path0, const string& path1 /*= "" */, const string& path2 /*= "" */, const string& path3 /*= ""*/) {
    string resolvedDevice;
    string resolvedTail;
    bool resolvedAbsolute = false;

    std::regex rx("%([^%]+)%");
    string innerPath0 = std::regex_replace(path0, rx, [](const std::smatch& m) {
      return process.getEnv(m[1].str());
    });

    int argv = 1;
    if (!path1.empty()) argv++;
    if (!path2.empty()) argv++;
    if (!path3.empty()) argv++;
    for (int i = argv - 1; i >= -1; i--) {
      string path;
      if (i >= 0) {
        if (i == 0) path = innerPath0;
        else if (i == 1) path = path1;
        else if (i == 2) path = path2;
        else if (i == 3) path = path3;
      }
      else if (resolvedDevice.empty()) {
        path = process.cwd();
      }
      else {
        // Windows has the concept of drive-specific current working
        // directories. If we've resolved a drive letter but not yet an
        // absolute path, get cwd for that drive. We're sure the device is not
        // a UNC path at this points, because UNC paths are always absolute.

        /* not support
        path = process.env["=" + resolvedDevice];
        // Verify that a drive-local cwd was found and that it actually points
        // to our drive. If not, default to the drive's root.
        if (s.toLower(s.slice(path, 0, 3)) !=
          s.toLower(resolvedDevice) + "\\") {
          path = resolvedDevice + "\\";
        }*/
      }

      // Skip empty entries
      if (path.empty()) {
        continue;
      }

      int len = path.length();
      int rootEnd = 0;
      char code = path[0];
      string device = "";
      bool isAbsolute = false;

      // Try to match a root
      if (len > 1) {
        if (code == 47/*/*/ || code == 92/*\*/) {
          // Possible UNC root

          // If we started with a separator, we know we at least have an
          // absolute path of some kind (UNC or otherwise)
          isAbsolute = true;

          code = path[1];
          if (code == 47/*/*/ || code == 92/*\*/) {
            // Matched double path separator at beginning
            int j = 2;
            int last = j;
            // Match 1 or more non-path separators
            for (; j < len; ++j) {
              code = path[j];
              if (code == 47/*/*/ || code == 92/*\*/)
                break;
            }
            if (j < len && j != last) {
              string firstPart = s.slice(path, last, j);
              // Matched!
              last = j;
              // Match 1 or more path separators
              for (; j < len; ++j) {
                code = path[j];
                if (code != 47/*/*/ && code != 92/*\*/)
                  break;
              }
              if (j < len && j != last) {
                // Matched!
                last = j;
                // Match 1 or more non-path separators
                for (; j < len; ++j) {
                  code = path[j];
                  if (code == 47/*/*/ || code == 92/*\*/)
                    break;
                }
                if (j == len) {
                  // We matched a UNC root only

                  device = "\\\\" + firstPart + "\\" + s.slice(path, last);
                  rootEnd = j;
                }
                else if (j != last) {
                  // We matched a UNC root with leftovers

                  device = "\\\\" + firstPart + "\\" + s.slice(path, last, j);
                  rootEnd = j;
                }
              }
            }
          }
          else {
            rootEnd = 1;
          }
        }
        else if ((code >= 65/*A*/ && code <= 90/*Z*/) ||
          (code >= 97/*a*/ && code <= 122/*z*/)) {
          // Possible device root

          code = path[1];
          if (path[1] == 58/*:*/) {
            device = s.slice(path, 0, 2);
            rootEnd = 2;
            if (len > 2) {
              code = path[2];
              if (code == 47/*/*/ || code == 92/*\*/) {
                // Treat separator following drive name as an absolute path
                // indicator
                isAbsolute = true;
                rootEnd = 3;
              }
            }
          }
        }
      }
      else if (code == 47/*/*/ || code == 92/*\*/) {
        // `path` contains just a path separator
        rootEnd = 1;
        isAbsolute = true;
      }

      if (device.length() > 0 &&
        resolvedDevice.length() > 0 &&
        s.toLower(device) != s.toLower(resolvedDevice)) {
        // This path points to another device so it is not applicable
        continue;
      }

      if (resolvedDevice.empty() && device.length() > 0) {
        resolvedDevice = device;
      }
      if (!resolvedAbsolute) {
        resolvedTail = s.slice(path, rootEnd) + "\\" + resolvedTail;
        resolvedAbsolute = isAbsolute;
      }

      if (resolvedDevice.length() > 0 && resolvedAbsolute) {
        break;
      }
    }

    // At this point the path should be resolved to a full absolute path,
    // but handle relative paths to be safe (might happen when process.cwd()
    // fails)

    // Normalize the tail path
    resolvedTail = normalizeStringWin32(resolvedTail, !resolvedAbsolute);

    string rst = (resolvedDevice + (resolvedAbsolute ? "\\" : "") + resolvedTail);
    if (rst.empty()) rst = ".";
    return rst;
  }

  Path &path = Path::instance();
}
