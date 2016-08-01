#pragma once
#include "stream-resource.h"
#include "buffer.h"
#include "util.h"

namespace nodecpp {
  template <typename Req>
  class StreamReq {
  public:
    typedef void(*DoneCb)(Req* req, int status);

    explicit StreamReq(DoneCb cb) : cb_(cb) {}

    inline void Done(int status, const char* error_str = nullptr) {
      Req* req = static_cast<Req*>(this);
      if (error_str != nullptr) {
      }

      cb_(req, status);
    }

  private:
    DoneCb cb_;
  };

  class StreamBase : public StreamResource {
  public:
    enum Flags {
      kFlagNone = 0x0,
      kFlagHasWritev = 0x1,
      kFlagNoShutdown = 0x2
    };

    virtual void* Cast() = 0;
    virtual bool IsAlive() = 0;
    virtual bool IsClosing() = 0;
    virtual bool IsIPCPipe();
    virtual int GetFD();

    virtual int ReadStart() = 0;
    virtual int ReadStop() = 0;

    inline void Consume() {
      CHECK_EQ(consumed_, false);
      consumed_ = true;
    }

    explicit StreamBase() : consumed_(false) {
    }

    virtual ~StreamBase() = default;

    int Shutdown();
    int WriteBuffer(const Buffer& buf);

  protected:
    // Libuv callbacks
    static void AfterShutdown(ShutdownWrap* req, int status);
    static void AfterWrite(WriteWrap* req, int status);

  private:
    bool consumed_;
  };
}