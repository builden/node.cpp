#pragma once
#include "nodecpp-def.h"
#include "stats-def.h"
#include "req-wrap.h"
#include "util.h"
#include "error.h"
#include "buffer.h"

namespace nodecpp {
#ifndef F_OK
#define F_OK 0
#endif
#ifndef R_OK
#define R_OK 4
#endif
#ifndef W_OK
#define W_OK 2
#endif
#ifndef X_OK
#define X_OK 1
#endif

  class FSReqWrap : public ReqWrap<uv_fs_t> {
  public:
    enum Ownership { COPY, MOVE };

    inline static FSReqWrap* New(
      const char* syscall,
      const char* data = nullptr,
      enum encoding encoding = UTF8,
      Ownership ownership = COPY) {
      const bool copy = (data != nullptr && ownership == COPY);
      const size_t size = copy ? 1 + strlen(data) : 0;
      FSReqWrap* that;
      char* const storage = new char[sizeof(*that) + size];
      that = new(storage) FSReqWrap(syscall, data, encoding);
      if (copy)
        that->data_ = static_cast<char*>(memcpy(that->inline_data(), data, size));
      return that;
    }

    FSReqWrap(const char* syscall,
      const char* data,
      enum encoding encoding);
    ~FSReqWrap() { ReleaseEarly(); }

    inline void Dispose();
    void ReleaseEarly() {
      if (data_ != inline_data()) {
        delete[] data_;
        data_ = nullptr;
      }
    }

    const char* syscall() const { return syscall_; }
    const char* data() const { return data_; }
    const enum encoding encoding_;

    function<void(const Error&)> onComplete = nullptr;
    function<void(const Error&, const Stats&)> onCompleteStats = nullptr;
    function<void(const Error&, int)> onCompleteResult = nullptr;

  private:
    void* operator new(size_t /*size*/) = delete;
    void* operator new(size_t /*size*/, char* storage) { return storage; }
    char* inline_data() { return reinterpret_cast<char*>(this + 1); }

    const char* syscall_;
    const char* data_;

    DISALLOW_COPY_AND_ASSIGN(FSReqWrap);
  };

  // This struct is only used on sync fs calls.
  // For async calls FSReqWrap is used.
  class fs_req_wrap {
  public:
    fs_req_wrap() {}
    ~fs_req_wrap() { uv_fs_req_cleanup(&req); }
    uv_fs_t req;

  private:
    DISALLOW_COPY_AND_ASSIGN(fs_req_wrap);
  };

  static void After(uv_fs_t *req);

#define ASYNC_DEST_CALL(func, req_wrap, dest, encoding, ...)                  \
  int err = uv_fs_ ## func(uv_default_loop(),                                 \
                           &req_wrap->req_,                                   \
                           __VA_ARGS__,                                       \
                           After);                                            \
  req_wrap->Dispatched();                                                     \
  if (err < 0) {                                                              \
    uv_fs_t* uv_req = &req_wrap->req_;                                        \
    uv_req->result = err;                                                     \
    uv_req->path = nullptr;                                                   \
    After(uv_req);                                                            \
    req_wrap = nullptr;                                                       \
  } else {                                                                    \
  }

#define ASYNC_CALL(func, req, encoding, ...)                                  \
  ASYNC_DEST_CALL(func, req, nullptr, encoding, __VA_ARGS__)                  \

#define SYNC_DEST_CALL(func, path, dest, ...)                                 \
  fs_req_wrap req_wrap;                                                       \
  int err = uv_fs_ ## func(uv_default_loop(),                                 \
                         &req_wrap.req,                                       \
                         __VA_ARGS__,                                         \
                         nullptr);                                            \
  if (err < 0) {                                                              \
    throw Error(err);                                                         \
  }                                                                           \

#define SYNC_CALL(func, path, ...)                                            \
  SYNC_DEST_CALL(func, path, nullptr, __VA_ARGS__)                            \

#define SYNC_REQ req_wrap.req

#define SYNC_RESULT err

  void Access(const string& path, int mode, FSReqWrap* reqWrap = nullptr);

  void Close(int fd, FSReqWrap* reqWrap = nullptr);

  void Stat(const string& path, FSReqWrap* reqWrap);
  Stats Stat(const string& path);
  void LStat(const string& path, FSReqWrap* reqWrap);
  Stats LStat(const string& path);
  void FStat(int fd, FSReqWrap* reqWrap);
  Stats FStat(int fd);

  // The type argument can be set to 'dir', 'file', or 'junction' 
  void Symlink(const string& target, const string& path, const string& type = "file", FSReqWrap* reqWrap = nullptr);
  void Symlink(const string& target, const string& path, FSReqWrap* reqWrap = nullptr);

  void Link(const string& srcPath, const string& destPath, FSReqWrap* reqWrap = nullptr);
  void ReadLink(const string& path, FSReqWrap* reqWrap);
  string ReadLink(const string& path);

  void Rename(const string& oldPath, const string& newPath, FSReqWrap* reqWrap = nullptr);

  void Unlink(const string& path, FSReqWrap* reqWrap = nullptr);
  void RMDir(const string& path, FSReqWrap* reqWrap = nullptr);
  void MKDir(const string& path, int mode, FSReqWrap* reqWrap = nullptr);
  void MKDir(const string& path, FSReqWrap* reqWrap = nullptr);

  void Open(const string& path, int flags, int mode, FSReqWrap* reqWrap);
  int Open(const string& path, int flags, int mode);

  /*
  void WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len, int64_t pos, FSReqWrap* reqWrap);
  void WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len, FSReqWrap* reqWrap);
  int WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len, int64_t pos);
  int WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len);
  */
  Stats BuildStatsObject(const uv_stat_t* s);
}