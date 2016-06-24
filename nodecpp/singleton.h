#ifndef __SINGLETON_H__
#define __SINGLETON_H__

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
      static T ins;
      return ins;
    }

  protected:
    explicit Singleton<T>() = default;
    ~Singleton<T>() = default;
  };
}

#endif // !__SINGLETON_H__
