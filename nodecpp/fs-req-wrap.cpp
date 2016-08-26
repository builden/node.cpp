#include "fs-req-wrap.h"
#include "fmt/format.h"
#include "iconv.h"

namespace nodecpp {

  FSReqWrap::FSReqWrap(const char* syscall, const char* data, enum encoding encoding)
    : ReqWrap(AsyncWrap::PROVIDER_FSREQWRAP),
      encoding_(encoding),
      syscall_(syscall),
      data_(data) {

  }

  void FSReqWrap::Dispose() {
    this->~FSReqWrap();
    delete[] reinterpret_cast<char*>(this);
  }

  void After(uv_fs_t *req) {
    FSReqWrap* req_wrap = static_cast<FSReqWrap*>(req->data);
    CHECK_EQ(&req_wrap->req_, req);
    req_wrap->ReleaseEarly();  // Free memory that's no longer used now.

    Stats stats;
    // there is always at least one argument. "error"
    int argc = 2;

    // error value is empty or null for non-error.
    // argv[0] = Null(env->isolate());

    svec_t names;
    switch (req->fs_type) {
      // These all have no data to pass.
    case UV_FS_ACCESS:
    case UV_FS_CLOSE:
    case UV_FS_RENAME:
    case UV_FS_UNLINK:
    case UV_FS_RMDIR:
    case UV_FS_MKDIR:
    case UV_FS_FTRUNCATE:
    case UV_FS_FSYNC:
    case UV_FS_FDATASYNC:
    case UV_FS_LINK:
    case UV_FS_SYMLINK:
    case UV_FS_CHMOD:
    case UV_FS_FCHMOD:
    case UV_FS_CHOWN:
    case UV_FS_FCHOWN:
      // These, however, don't.
      argc = 1;
      break;

    case UV_FS_UTIME:
    case UV_FS_FUTIME:
      argc = 0;
      break;

    case UV_FS_OPEN:
      // argv[1] = Integer::New(env->isolate(), req->result);
      break;

    case UV_FS_WRITE:
      // argv[1] = Integer::New(env->isolate(), req->result);
      break;

    case UV_FS_STAT:
    case UV_FS_LSTAT:
    case UV_FS_FSTAT:
      if (req->ptr) stats = BuildStatsObject(static_cast<const uv_stat_t*>(req->ptr));
      break;

    case UV_FS_MKDTEMP:
/*
      link = StringBytes::Encode(env->isolate(),
        static_cast<const char*>(req->path),
        req_wrap->encoding_);
      if (link.IsEmpty()) {
        argv[0] = UVException(env->isolate(),
          UV_EINVAL,
          req_wrap->syscall(),
          "Invalid character encoding for filename",
          req->path,
          req_wrap->data());
      }
      else {
        argv[1] = link;
      }*/
      break;

    case UV_FS_READLINK:
/*
      link = StringBytes::Encode(env->isolate(),
        static_cast<const char*>(req->ptr),
        req_wrap->encoding_);
      if (link.IsEmpty()) {
        argv[0] = UVException(env->isolate(),
          UV_EINVAL,
          req_wrap->syscall(),
          "Invalid character encoding for link",
          req->path,
          req_wrap->data());
      }
      else {
        argv[1] = link;
      }*/
      break;

    case UV_FS_REALPATH:
/*
      link = StringBytes::Encode(env->isolate(),
        static_cast<const char*>(req->ptr),
        req_wrap->encoding_);
      if (link.IsEmpty()) {
        argv[0] = UVException(env->isolate(),
          UV_EINVAL,
          req_wrap->syscall(),
          "Invalid character encoding for link",
          req->path,
          req_wrap->data());
      }
      else {
        argv[1] = link;
      }*/
      break;

    case UV_FS_READ:
      // Buffer interface
      // argv[1] = Integer::New(env->isolate(), req->result);
      break;

    case UV_FS_SCANDIR:
    {
      if (req->result < 0) break;
      int r = 0;

      for (int i = 0; ; i++) {
        uv_dirent_t ent;

        r = uv_fs_scandir_next(req, &ent);
        if (r == UV_EOF)
          break;
        if (r != 0) {
          throw Error(r);
          break;
        }

        string filename = ent.name;
        if (filename.empty()) {
          throw Error(UV_EINVAL);
          break;
        }
        names.emplace_back(iconv.utf8ToStr(filename));
      }
    }
    break;

    default:
      // CHECK(0 && "Unhandled eio response");
      break;
    }

    // req_wrap->MakeCallback(env->oncomplete_string(), argc, argv);
    if (argc == 1 && req_wrap->onComplete) req_wrap->onComplete(Error(req->result));
    else if (argc == 2 && req_wrap->onCompleteStats) req_wrap->onCompleteStats(Error(req->result), stats);
    else if (argc == 2 && req_wrap->onCompleteResult) req_wrap->onCompleteResult(Error(req->result), req->result);
    else if (argc == 2 && req_wrap->onCompleteReadDir) req_wrap->onCompleteReadDir(Error(req->result), names);

    uv_fs_req_cleanup(&req_wrap->req_);
    req_wrap->Dispose();
  }

  void Access(const string& path, int mode, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap != nullptr) {
      ASYNC_CALL(access, reqWrap, UTF8, path.c_str(), mode);
    }
    else {
      SYNC_CALL(access, path.c_str(), path.c_str(), mode);
    }
  }

  void Close(int fd, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap != nullptr) {
      ASYNC_CALL(close, reqWrap, UTF8, fd);
    }
    else {
      SYNC_CALL(close, 0, fd);
    }
  }

  void Stat(const string& path, FSReqWrap* reqWrap) {
    ASYNC_CALL(stat, reqWrap, UTF8, path.c_str());
  }

  Stats Stat(const string& path) {
    SYNC_CALL(stat, path.c_str(), path.c_str());
    return BuildStatsObject(static_cast<const uv_stat_t*>(SYNC_REQ.ptr));
  }

  void LStat(const string& path, FSReqWrap* reqWrap) {
    ASYNC_CALL(lstat, reqWrap, UTF8, path.c_str());
  }

  Stats LStat(const string& path) {
    SYNC_CALL(lstat, path.c_str(), path.c_str());
    return BuildStatsObject(static_cast<const uv_stat_t*>(SYNC_REQ.ptr));
  }

  void FStat(int fd, FSReqWrap* reqWrap) {
    ASYNC_CALL(fstat, reqWrap, UTF8, fd);
  }

  Stats FStat(int fd) {
    SYNC_CALL(fstat, 0, fd);
    return BuildStatsObject(static_cast<const uv_stat_t*>(SYNC_REQ.ptr));
  }

  void Symlink(const string& target, const string& path, const string& type /*= "file"*/, FSReqWrap* reqWrap /*= nullptr*/) {
    int flags = 0;
    if (type == "dir") {
      flags |= UV_FS_SYMLINK_DIR;
    } 
    else if (type == "junction") {
      flags |= UV_FS_SYMLINK_JUNCTION;
    }
    if (reqWrap) {
      ASYNC_DEST_CALL(symlink, reqWrap, path.c_str(), UTF8, target.c_str(), path.c_str(), flags);
    }
    else {
      SYNC_DEST_CALL(symlink, target.c_str(), path.c_str(), target.c_str(), path.c_str(), flags);
    }
  }

  void Symlink(const string& target, const string& path, FSReqWrap* reqWrap /*= "file"*/) {
    Symlink(target, path, "file", reqWrap);
  }

  void Link(const string& srcPath, const string& destPath, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap) {
      ASYNC_DEST_CALL(link, reqWrap, destPath.c_str(), UTF8, srcPath.c_str(), destPath.c_str());
    }
    else {
      SYNC_DEST_CALL(link, srcPath.c_str(), destPath.c_str(), srcPath.c_str(), destPath.c_str());
    }
  }

  void ReadLink(const string& path, FSReqWrap* reqWrap) {
    ASYNC_CALL(readlink, reqWrap, UTF8, path.c_str());
  }

  string ReadLink(const string& path) {
    SYNC_CALL(readlink, path.c_str(), path.c_str());
    const char* link_path = static_cast<const char*>(SYNC_REQ.ptr);
    if (link_path == nullptr) {
      throw Error("readlink Invalid character encoding for link");
    }
    return link_path;
  }

  void Rename(const string& oldPath, const string& newPath, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap) {
      ASYNC_DEST_CALL(rename, reqWrap, newPath.c_str(), UTF8, oldPath.c_str(), newPath.c_str());
    }
    else {
      SYNC_DEST_CALL(rename, oldPath.c_str(), newPath.c_str(), oldPath.c_str(), newPath.c_str());
    }
  }

  void Unlink(const string& path, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap) {
      ASYNC_CALL(unlink, reqWrap, UTF8, path.c_str());
    }
    else {
      SYNC_CALL(unlink, path.c_str(), path.c_str());
    }
  }

  void RMDir(const string& path, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap) {
      ASYNC_CALL(rmdir, reqWrap, UTF8, path.c_str());
    }
    else {
      SYNC_CALL(rmdir, path.c_str(), path.c_str());
    }
  }

  void MKDir(const string& path, FSReqWrap* reqWrap /*= nullptr*/) {
    MKDir(path, 0777, reqWrap);
  }

  void MKDir(const string& path, int mode, FSReqWrap* reqWrap /*= nullptr*/) {
    if (reqWrap) {
      ASYNC_CALL(mkdir, reqWrap, UTF8, path.c_str(), mode);
    }
    else {
      SYNC_CALL(mkdir, path.c_str(), path.c_str(), mode);
    }
  }

  void ReadDir(const string& path, FSReqWrap* reqWrap) {
    ASYNC_CALL(scandir, reqWrap, UTF8, path.c_str(), 0 /*flags*/);
  }

  svec_t ReadDir(const string& path) {
    SYNC_CALL(scandir, path.c_str(), path.c_str(), 0 /*flags*/);
    CHECK_GE(SYNC_REQ.result, 0);
    int r = 0;
    svec_t names;

    for (int i = 0; ; i++) {
      uv_dirent_t ent;

      r = uv_fs_scandir_next(&SYNC_REQ, &ent);
      if (r == UV_EOF)
        break;
      if (r != 0)
        throw Error(r);

      string filename = ent.name;
      if (filename.empty()) {
        throw Error(UV_EINVAL);
      }

      names.emplace_back(iconv.utf8ToStr(filename));
    }

    return names;
  }

  void Open(const string& path, int flags, int mode, FSReqWrap* reqWrap) {
    ASYNC_CALL(open, reqWrap, UTF8, path.c_str(), flags, mode);
  }

  int Open(const string& path, int flags, int mode) {
    SYNC_CALL(open, path.c_str(), path.c_str(), flags, mode);
    return SYNC_RESULT;
  }

  void Read(int fd, Buffer& buffer, uint32_t off, uint32_t len, int64_t pos, FSReqWrap* reqWrap) {
    if (off >= buffer.length()) throw Error("Offset is out of bounds");
    const char* buf = buffer.data() + off;
    uv_buf_t uvbuf = uv_buf_init(const_cast<char*>(buf), len);
    ASYNC_CALL(read, reqWrap, UTF8, fd, &uvbuf, 1, pos);
  }

  uint32_t Read(int fd, Buffer& buffer, uint32_t off, uint32_t len, int64_t pos) {
    if (off >= buffer.length()) throw Error("Offset is out of bounds");
    const char* buf = buffer.data() + off;
    uv_buf_t uvbuf = uv_buf_init(const_cast<char*>(buf), len);
    SYNC_CALL(read, 0, fd, &uvbuf, 1, pos);
    return SYNC_RESULT;
  }

  int WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len, int64_t pos /*= -1*/) {
    const char* buf = buffer.data() + off;
    uv_buf_t uvbuf = uv_buf_init(const_cast<char*>(buf), len);
    SYNC_CALL(write, nullptr, fd, &uvbuf, 1, pos);
    return SYNC_RESULT;
  }

  void WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len, int64_t pos, FSReqWrap* reqWrap) {
    const char* buf = buffer.data() + off;
    uv_buf_t uvbuf = uv_buf_init(const_cast<char*>(buf), len);
    ASYNC_CALL(write, reqWrap, UTF8, fd, &uvbuf, 1, pos);
  }

  void WriteBuffer(int fd, const Buffer& buffer, uint32_t off, uint32_t len, FSReqWrap* reqWrap) {
    WriteBuffer(fd, buffer, off, len, -1, reqWrap);
  }

  Stats BuildStatsObject(const uv_stat_t* s) {
    Stats stats;
    stats.dev = s->st_mode;
    stats.mode = s->st_mode;
    stats.nlink = s->st_nlink;
    stats.uid = s->st_uid;
    stats.gid = s->st_gid;
    stats.rdev = s->st_rdev;
    stats.blksize = s->st_blksize;
    stats.ino = s->st_ino;
    stats.size = s->st_size;
    stats.atime = Moment(s->st_atim.tv_sec, s->st_atim.tv_nsec);
    stats.mtime = Moment(s->st_mtim.tv_sec, s->st_mtim.tv_nsec);
    stats.ctime = Moment(s->st_ctim.tv_sec, s->st_ctim.tv_nsec);
    stats.birthtime = Moment(s->st_birthtim.tv_sec, s->st_birthtim.tv_nsec);
    return stats;
  }

}