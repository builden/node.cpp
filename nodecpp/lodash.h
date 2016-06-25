/**
 * count(list) - 支持array, vector, set等容器
 * count(list, cb) - cb(x) 返回true +1
 * map(list, cb) - cb(x) 单项运算后，构造新的vector，容器内的类型是通过decltype推导出来的
 * select/filter(list, cb) - cb(x) 返回true，则把该项放如新vector中
 * reject(list, cb) - 和filter相反
 * sample(list) - 随机选一项
 * sort(list) - 重新排序
 * shuffle(list) - 洗牌
 * min(list) - 列表中的最小值
 * max(list) - 列表中的最大值
 * min_by(list, cb) - 
 * max_by(list, cb) -
 * inject/reduce(list, init, cb)
 *
 * formatList - 格式化列表
 */
#ifndef __LODASH_H__
#define __LODASH_H__
#include "nodecpp-def.h"
#include "singleton.h"
#include <time.h>

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

namespace nodecpp {
  using std::begin;
  using std::end;
  using std::stringstream;

  class Lodash : public Singleton<Lodash> {
  public:
    template<typename T>
    static auto _pure_typeof(T x) -> typename std::remove_const<typename std::remove_reference<decltype(x)>::type>::type { return decltype(_pure_typeof(x))(); };

    template<typename L>
    static void each(const L& list, function<void(decltype(*begin(list))&)> func) {
      for (auto& i : list) func(i);
    }

    template<typename L>
    static size_t count(const L& list) {
      return (size_t)std::distance(begin(list), end(list));
    }

    template<typename L>
    static size_t count(const L& list, function<bool(decltype(*begin(list)))> func) {
      return (size_t)std::count_if(begin(list), end(list), func);
    }

    template<typename L, typename F>
    static auto map(const L& list, F func) -> vector<decltype(func(*begin(list)))>  {
      typedef decltype(func(*begin(list))) FR;
      vector<FR> result;
      result.resize(count(list));
      std::transform(begin(list), end(list), result.begin(), func);
      return result;
    }

    template<typename L>
    static auto select(const L& list, function<bool(decltype(*begin(list)))> func) -> vector<decltype(_pure_typeof(*begin(list)))> {
      vector<decltype(_pure_typeof(*begin(list)))> result;
      std::copy_if(begin(list), end(list), std::back_inserter(result), func);
      return result;
    }

    template<typename L>
    static auto filter(const L& list, function<bool(decltype(*begin(list)))> func) -> vector<decltype(_pure_typeof(*begin(list)))> {
      return select(list, func);
    }

    template<typename L>
    static auto reject(const L& list, function<bool(decltype(*begin(list)))> func) -> vector<decltype(_pure_typeof(*begin(list)))> {
      return select(list, [&](decltype(*begin(list)) i) { return !func(i); });
    }

    template<typename L>
    static auto sample(const L& list, size_t n) -> vector<decltype(_pure_typeof(*begin(list)))> {
      size_t avail = count(list);
      n = std::min(n, avail);

      vector<decltype(_pure_typeof(*begin(list)))> result;
      result.reserve(n);

      for (auto& i : list) {
        if (n == 0) break;
        if ((std::rand() % avail) < n) {
          result.push_back(i);
          --n;
        }
        --avail;
      }
      return result;
    }

    template<typename L>
    static auto sample(const L& list) -> decltype(_pure_typeof(*begin(list))) {
      size_t avail = count(list);
      for (auto& i : list) {
        if (std::rand() % avail == 0) return i;
      }
      return _pure_typeof(*begin(list));
    }

    template<typename L>
    static auto sort(const L& list) -> vector<decltype(_pure_typeof(*begin(list)))> {
      vector<decltype(_pure_typeof(*begin(list)))> result;
      result.reserve(count(list));
      std::copy(begin(list), end(list), std::back_inserter(result));
      std::stable_sort(result.begin(), result.end());
      return result;
    }

    template<typename L>
    static auto shuffle(const L& list) -> vector<decltype(_pure_typeof(*begin(list)))> {
      vector<decltype(_pure_typeof(*begin(list)))> result;
      result.reserve(count(list));
      std::copy(begin(list), end(list), std::back_inserter(result));
      // 确保随机
      srand((unsigned int)time(0));
      std::random_shuffle(result.begin(), result.end());
      return result;
    }

    template<typename L>
    static auto min(const L& list) -> decltype(_pure_typeof(*begin(list))) {
      typedef decltype(_pure_typeof(*begin(list))) IT;
      IT result = *begin(list);
      each(list, [&](IT x) { if (x < result) result = x; });
      return result;
    }

    template<typename L>
    static auto max(const L& list) -> decltype(_pure_typeof(*begin(list))) {
      typedef decltype(_pure_typeof(*begin(list))) IT;
      IT result = *begin(list);
      each(list, [&](IT x) { if (x > result) result = x; });
      return result;
    }

    template<typename L, typename F>
    static auto min_by(const L& list, F func) -> decltype(_pure_typeof(*begin(list))) {
      typedef decltype(func(*begin(list))) FR;
      if (count(list) == 0) return FR();
      decltype(_pure_typeof(*begin(list))) result = *begin(list);
      FR val = func(*begin(list));
      each(list, [&](auto x) { if (x != result) { FR tmp = func(x); if (func(x) < val) { result = x; val = tmp; } }});
      return result;
    }

    template<typename L, typename F>
    static auto max_by(const L& list, F func) -> decltype(_pure_typeof(*begin(list))) {
      typedef decltype(func(*begin(list))) FR;
      if (count(list) == 0) return FR();
      decltype(_pure_typeof(*begin(list))) result = *begin(list);
      FR val = func(*begin(list));
      each(list, [&](auto x) { if (x != result) { FR tmp = func(x); if (func(x) > val) { result = x; val = tmp; } }});
      return result;
    }

    template<typename L, typename F>
    static auto inject(const L& list, F func) -> decltype(func(*begin(list), *begin(list))) {
      decltype(func(*begin(list), *begin(list))) result;
      int i = 0;
      for (auto& x : list) {
        if (!i) {
          result = x;
          ++i;
        }
        else {
          result = func(result, x);
        }
      }
      return result;
    }

    template<typename L, typename R, typename F>
    static auto inject(const L& list, R init, F func) -> decltype(func(init, *begin(list))) {
      R result = init;
      for (auto& x : list) result = func(result, x);
      return result;
    }

    template<typename L, typename F>
    static auto reduce(const L& list, F func) -> decltype(func(*begin(list), *begin(list))) {
      return inject(list, func);
    }

    template<typename L, typename R, typename F>
    static auto reduce(const L& list, R init, F func) -> decltype(func(init, *begin(list))) {
      return inject(list, init, func);
    }

    template<typename L>
    static string formatList(const L& list, bool isString = false) {
      stringstream ss;
      ss << "[";
      size_t len = count(list);
      size_t i = 0;
      for (auto& v : list) {
        isString ? (ss << "\"" << v << "\"") : (ss << v);
        if (++i != len) ss << ", ";
      }
      ss << "]";
      return ss.str();
    }

    template<typename L>
    static auto slice(const L& list, int start)->vector<decltype(_pure_typeof(*begin(list)))> {
      int len = count(list);
      return slice(list, start, len);
    }

    template<typename L>
    static auto slice(const L& list, int start, int end) -> vector<decltype(_pure_typeof(*begin(list)))> {
      size_t len = count(list);
      if (end < 0) end += len;
      vector<decltype(_pure_typeof(*begin(list)))> result;
      if (end > start && start < static_cast<int>(len)) result.assign(begin(list) + start, begin(list) + end);
      return result;
    }

    template<typename L, typename R>
    static int indexOf(const L& list, R value, uint32_t fromIndex = 0) {
      uint32_t len = count(list);
      for (uint32_t i = fromIndex; i < len; ++i) {
        if (list[i] == value) return i;
      }
      return -1;
    }

    template<typename L, typename R>
    static bool includes(const L& list, R value, uint32_t fromIndex = 0) {
      return indexOf(list, value, fromIndex) != -1;
    }
  };

  extern Lodash &_;
}

#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // !__LODASH_H__
