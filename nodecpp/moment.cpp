#include "moment.h"

#include <sys/timeb.h> 

namespace nodecpp {
  string Moment::millisecond_to_str(int64_t milliseconds) {
    std::chrono::milliseconds ms(milliseconds);
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> t1(ms);
    std::time_t t = std::chrono::system_clock::to_time_t(t1);

    std::ostringstream oss;
    oss.fill('0');
    auto const msecs = ms.count() % 1000;
    oss << std::put_time(&localtime(t), "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << msecs;
    return oss.str();
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

  Moment::Moment(const uv_timespec_t& timespec) {
    milliseconds_ = timespec.tv_sec;
    milliseconds_ = milliseconds_ * 1000 + timespec.tv_nsec / 1000000;
  }

  string Moment::format() {
    return millisecond_to_str(milliseconds_);
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
