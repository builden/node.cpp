#ifndef __CORE_MATH_H__
#define __CORE_MATH_H__

#include "singleton.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

namespace nodecpp {
  class CoreMath : public Singleton<CoreMath> {
  public:
    
    static int abs(int x);
    static int ceil(double x);  // 上舍入 ceil(1.1) == 2
    static int floor(double x); // 下舍入 floor(1.9) == 1
    static int round(double x); // 四舍五入
    static double random();

    template<typename T>
    static auto min(T x, T y) -> decltype(x) {
      return ((x > y) ? y : x);
    };

    template<typename T>
    static auto max(T x, T y) -> decltype(x) {
      return ((x > y) ? x : y);
    }
  };
  extern CoreMath &Math;
}

#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // !__CORE_MATH_H__
