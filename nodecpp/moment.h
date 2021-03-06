#ifndef __MOMENT_H__
#define __MOMENT_H__
#include "nodecpp-def.h"
#include <chrono>

namespace nodecpp {
  class Moment {
  public:
    Moment();
    Moment(uint64_t milliseconds);  // ����
    Moment(uint32_t seconds);       // ��
    Moment(uint32_t seconds, uint32_t nanoseconds);

    string format(const string& fmtStr = "");

    uint64_t valueOf();  // ����
    uint32_t unix();     // ��

  private:
    typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;

    // ��
    std::time_t systemtime_now();
    tm localtime(const std::time_t &time);
    string localtime_formatted(const std::time_t &time_snapshot, const string &time_format);

    string millisecond_to_str(int64_t milliseconds, const string& fmtStr);
    string put_time(const struct tm *tmb, const char *c_time_format);

    uint64_t milliseconds_;
  };

  Moment moment();
}

#endif // !__MOMENT_H__
