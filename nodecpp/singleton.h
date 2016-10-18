#pragma once

#include <memory>
using std::unique_ptr;

namespace nodecpp {
  class noncopyable {
  protected:
    noncopyable() = default;
    ~noncopyable() = default;
  private:
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
  };

  template<class T>
  class Singleton : private noncopyable {
  public:
    static T& instance() {
      if (!ins_.get()) {
        ins_.reset(new T);
      }
      return *(ins_.get());
    }

    static void destroy() {
      if (ins_) {
        ins_.reset();
      }
    }

  protected:
    explicit Singleton<T>() = default;
    ~Singleton<T>() = default;
    static unique_ptr<T> ins_;
  };

  template<typename T>
  std::unique_ptr<T> Singleton<T>::ins_ = nullptr;
}
