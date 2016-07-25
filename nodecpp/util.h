#pragma once
#include <signal.h>
// #include <stddef.h>
#include <stdio.h>
// #include <stdlib.h>

namespace nodecpp {
  // BINARY is a deprecated alias of LATIN1.
  enum encoding { ASCII, UTF8, BASE64, UCS2, BINARY, HEX, BUFFER, LATIN1 = BINARY };

  void Abort();
  void Assert(const char* const (*args)[4]);
  void DumpBacktrace(FILE* fp);

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                    \
  void operator=(const TypeName&) = delete;                                   \
  void operator=(TypeName&&) = delete;                                        \
  TypeName(const TypeName&) = delete;                                         \
  TypeName(TypeName&&) = delete

#define ABORT_NO_BACKTRACE() raise(SIGABRT)
#define ABORT() nodecpp::Abort()

#define LIKELY(expr) expr
#define UNLIKELY(expr) expr
#define PRETTY_FUNCTION_NAME ""

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define CHECK(expr)                                                           \
  do {                                                                        \
    if (UNLIKELY(!(expr))) {                                                  \
      static const char* const args[] = { __FILE__, STRINGIFY(__LINE__),      \
                                          #expr, PRETTY_FUNCTION_NAME };      \
      nodecpp::Assert(&args);                                                    \
    }                                                                         \
  } while (0)

#ifdef NDEBUG
#define ASSERT(expr)
#else
#define ASSERT(expr) CHECK(expr)
#endif

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_GE(a, b) ASSERT((a) >= (b))
#define ASSERT_GT(a, b) ASSERT((a) > (b))
#define ASSERT_LE(a, b) ASSERT((a) <= (b))
#define ASSERT_LT(a, b) ASSERT((a) < (b))
#define ASSERT_NE(a, b) ASSERT((a) != (b))

#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_NE(a, b) CHECK((a) != (b))

#define UNREACHABLE() ABORT()

  // The helper is for doing safe downcasts from base types to derived types.
  template <typename Inner, typename Outer>
  class ContainerOfHelper {
  public:
    inline ContainerOfHelper(Inner Outer::*field, Inner* pointer);
    template <typename TypeName>
    inline operator TypeName*() const;
  private:
    Outer* const pointer_;
  };

  // Calculate the address of the outer (i.e. embedding) struct from
  // the interior pointer to a data member.
  template <typename Inner, typename Outer>
  inline ContainerOfHelper<Inner, Outer> ContainerOf(Inner Outer::*field,
    Inner* pointer);

  template <typename Inner, typename Outer>
  ContainerOfHelper<Inner, Outer>::ContainerOfHelper(Inner Outer::*field,
    Inner* pointer)
    : pointer_(reinterpret_cast<Outer*>(
      reinterpret_cast<uintptr_t>(pointer) -
      reinterpret_cast<uintptr_t>(&(static_cast<Outer*>(0)->*field)))) {
  }

  template <typename Inner, typename Outer>
  template <typename TypeName>
  ContainerOfHelper<Inner, Outer>::operator TypeName*() const {
    return static_cast<TypeName*>(pointer_);
  }

  template <typename Inner, typename Outer>
  inline ContainerOfHelper<Inner, Outer> ContainerOf(Inner Outer::*field,
    Inner* pointer) {
    return ContainerOfHelper<Inner, Outer>(field, pointer);
  }
}