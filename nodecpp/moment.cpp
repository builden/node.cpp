#include "moment.h"
#include <sys/timeb.h>
#include "fmt/format.h"
#include "std-regex-ex.h"

namespace nodecpp {
  string Moment::millisecond_to_str(int64_t milliseconds, const string& fmtStr) {
    std::chrono::milliseconds ms(milliseconds);
    std::chrono::system_clock::time_point t1(ms);
    // std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> t1(ms);
    std::time_t t = std::chrono::system_clock::to_time_t(t1);

    string _fmtStr = fmtStr;
    if (_fmtStr.empty()) _fmtStr = "YYYY-MM-DD HH:mm:ss.SSS";
    auto const msecs = ms.count() % 1000;
    tm _tm = localtime(t);

    std::regex re(R"((YYYY|YY|MM|DD|HH|mm|ss|SSS))");
    string rst = std::regex_replace(_fmtStr, re, [&_tm, &msecs](const std::smatch& m) {
      string matched = m[1].str();
      if (matched == "YYYY") return fmt::format("{:04}", _tm.tm_year + 1900);
      else if (matched == "YY") return fmt::format("{:02}", (_tm.tm_year + 1900) % 100);
      else if (matched == "MM") return fmt::format("{:02}", _tm.tm_mon + 1);
      else if (matched == "DD") return fmt::format("{:02}", _tm.tm_mday);
      else if (matched == "HH") return fmt::format("{:02}", _tm.tm_hour);
      else if (matched == "mm") return fmt::format("{:02}", _tm.tm_min);
      else if (matched == "ss") return fmt::format("{:02}", _tm.tm_sec);
      else if (matched == "SSS") return fmt::format("{:03}", msecs);
      else return matched;
    });
    return rst;
  }

  string Moment::put_time(const struct tm *tmb, const char *c_time_format) {
    std::ostringstream oss;
    oss.fill('0');
    // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
    oss << std::put_time(const_cast<struct tm *> (tmb), c_time_format);
    return oss.str();
  }

  Moment::Moment() {
    system_time_point system_now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(system_now.time_since_epoch());
    milliseconds_ = ms.count();
  }

  Moment::Moment(uint64_t milliseconds) {
    milliseconds_ = milliseconds;
  }

  Moment::Moment(uint32_t seconds) {
    milliseconds_ = seconds;
    milliseconds_ *= 1000;
  }

  Moment::Moment(uint32_t seconds, uint32_t nanoseconds) {
    milliseconds_ = seconds;
    milliseconds_ = milliseconds_ * 1000 + nanoseconds / 1000000;
  }

  string Moment::format(const string& fmtStr) {
    return millisecond_to_str(milliseconds_, fmtStr);
  }

  uint64_t Moment::valueOf() {
    return milliseconds_;
  }

  std::time_t Moment::systemtime_now() {
    system_time_point system_now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(system_now);
  }

  tm Moment::localtime(const std::time_t &time) {
    struct tm tm_snapshot;
    localtime_s(&tm_snapshot, &time);
    return tm_snapshot;
  }

  string Moment::localtime_formatted(const std::time_t &time_snapshot, const string &time_format) {
    std::tm t = localtime(time_snapshot);
    return put_time(&t, time_format.c_str());
  }

  uint32_t Moment::unix() {
    return static_cast<uint32_t>(milliseconds_ / 1000);
  }

  Moment moment() {
    return Moment();
  }

}
