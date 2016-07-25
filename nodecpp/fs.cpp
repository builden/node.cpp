#include "fs.h"
#include "iconv.h"
#include "callback.h"
#include "fmt/format.h"

#include <uv.h>
#include <fcntl.h>
#include "fs-req-wrap.h"

namespace nodecpp {
  using OpenCbWrap = CallbackWrap<OpenCb_t>;

  void Fs::open(const string& path, const string& flags, OpenCb_t cb) {
    open(path, flags, 0, cb);
  }

  void Fs::open(const string& path, const string& flags, int mode, OpenCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteResult = cb;
    Open(path, stringToFlags(flags), mode, reqWrap);
  }

  int Fs::openSync(const string& path, const string& flags, int mode /*= 0*/) {
    return Open(path, stringToFlags(flags), mode);
  }

  void Fs::close(int fd, CloseCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Close(fd, reqWrap);
  }

  void Fs::closeSync(int fd) {
    Close(fd);
  }

  void Fs::readFile(const string& path, ReadCb_t cb) {
    // flags default 'r'
    uv_fs_t openReq;
    int fd = uv_fs_open(uv_default_loop(), &openReq, iconv.strToUtf8(path).c_str(), stringToFlags("r"), 0666, nullptr);
    if (fd < 0) {
      return cb(Error(fd), Buffer());
    }

    auto fsWrap = new FsWrap(fd);
    fsWrap->readFile(cb);
  }

  void Fs::readFile(const string& path, const string& encoding, ReadStrCb_t cb) {
    readFile(path, [&](const Error& err, const Buffer& buf) {
      cb(err, buf.toString(encoding));
    });
  }
  
  int Fs::stringToFlags(const string& flag) {
    if (flag == "r") return O_RDONLY;
    if (flag == "rs" || flag == "sr") return O_RDONLY; // | O_SYNC;
    if (flag == "r+") return O_RDWR;
    if (flag == "rs+" || flag == "sr+") return O_RDWR; // | O_SYNC;

    if (flag == "w") return O_TRUNC | O_CREAT | O_WRONLY;
    if (flag == "wx" || flag == "xw") return O_TRUNC | O_CREAT | O_RDWR | O_EXCL;

    if (flag == "w+") O_TRUNC | O_CREAT | O_RDWR;
    if (flag == "wx+" || flag == "xw+") return O_TRUNC | O_CREAT | O_RDWR | O_EXCL;

    if (flag == "a") return O_APPEND | O_CREAT | O_WRONLY;
    if (flag == "ax" || flag == "xa") return O_APPEND | O_CREAT | O_WRONLY | O_EXCL;

    if (flag == "a+") return O_APPEND | O_CREAT | O_RDWR;
    if (flag == "ax+" || flag == "xa+") return O_APPEND | O_CREAT | O_RDWR | O_EXCL;

    return O_RDONLY;
  }

  Buffer Fs::readFileSync(const string& path) {
    Buffer buf;
    uv_fs_t openReq;
    uv_loop_t *loop = uv_default_loop();
    int fd = uv_fs_open(loop, &openReq, iconv.strToUtf8(path).c_str(), stringToFlags("r"), 0666, nullptr);
    if (fd < 0) {
      uv_fs_req_cleanup(&openReq);
      throw Error(fd);
    }

    uv_fs_t stateReq;
    uv_fs_fstat(loop, &stateReq, fd, nullptr);
    uv_stat_t& statbuf = stateReq.statbuf;
    if (statbuf.st_size > 0) {
      uint32_t size = static_cast<uint32_t>(statbuf.st_size);
      uv_buf_t iov = uv_buf_init(new char[size], size);
      
      uv_fs_t readReq;
      uv_fs_read(loop, &readReq, fd, &iov, 1, 0, nullptr);
      buf.append(iov.base, iov.len);
      uv_fs_req_cleanup(&readReq);
    }

    uv_fs_req_cleanup(&stateReq);
    uv_fs_t closeReq;
    uv_fs_close(loop, &closeReq, fd, nullptr);
    uv_fs_req_cleanup(&closeReq);
    uv_fs_req_cleanup(&openReq);
    return buf;
  }

  string Fs::readFileSync(const string& path, const string& encoding) {
    Buffer buf = readFileSync(path);
    return buf.toString(encoding);
  }

  void Fs::writeFile(const string& path, const Buffer& data, WriteCb_t cb) {
    uv_fs_t openReq;
    int fd = uv_fs_open(uv_default_loop(), &openReq, iconv.strToUtf8(path).c_str(), stringToFlags("w"), 0666, nullptr);
    if (fd < 0) {
      return cb(Error(fd));
    }

    auto fsWrap = new FsWrap(fd);
    fsWrap->writeFile(data, cb);
  }

  void Fs::writeFileSync(const string& path, const Buffer& data) {
    uv_fs_t openReq;
    uv_loop_t *loop = uv_default_loop();
    int fd = uv_fs_open(loop, &openReq, iconv.strToUtf8(path).c_str(), stringToFlags("w"), 0666, nullptr);
    if (fd < 0) {
      uv_fs_req_cleanup(&openReq);
      throw Error(fd);
    }

    uv_fs_t writeReq;
    uv_buf_t iov = uv_buf_init(const_cast<char*>(data.data()), data.size());
    uv_fs_write(loop, &writeReq, fd, &iov, 1, 0, nullptr);

    uv_fs_req_cleanup(&writeReq);
    uv_fs_t closeReq;
    uv_fs_close(loop, &closeReq, fd, nullptr);
    uv_fs_req_cleanup(&closeReq);
    uv_fs_req_cleanup(&openReq);
    return;
  }

  void Fs::stat(const string& path, StatCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteStats = cb;
    Stat(path);
  }

  Stats Fs::statSync(const string& path) {
    return Stat(path);
  }

  void Fs::lstat(const string& path, StatCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteStats = cb;
    LStat(path);
  }

  Stats Fs::lstatSync(const string& path) {
    return LStat(path);
  }

  void Fs::fstat(int fd, StatCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteStats = cb;
    FStat(fd);
  }

  Stats Fs::fstat(int fd) {
    return FStat(fd);
  }

  void Fs::exists(const string& /*path*/, ExistsCb_t /*cb*/) {

  }

  bool Fs::existsSync(const string& path) {
    try {
      statSync(path);
      return true;
    }
    catch (const Error&) {
      return false;
    }
  }

  void Fs::mkdir(const string& path, MkdirCb_t cb) {
    mkdir(path, 0777, cb);
  }

  void Fs::mkdir(const string& path, int mode, MkdirCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    MKDir(path, mode, reqWrap);
  }

  void Fs::mkdirSync(const string& path, int mode /*= 0777*/) {
    MKDir(path, mode);
  }

  void Fs::rmdir(const string& path, RmdirCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    RMDir(path, reqWrap);
  }

  void Fs::rmdirSync(const string& path) {
    RMDir(path);
  }

  void Fs::unlink(const string& path, UnlinkCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Unlink(path, reqWrap);
  }

  void Fs::unlinkSync(const string& path) {
    Unlink(path);
  }

  void Fs::rename(const string& oldPath, const string& newPath, RenameCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Rename(oldPath, newPath, reqWrap);
  }

  void Fs::renameSync(const string& oldPath, const string& newPath) {
    Rename(oldPath, newPath);
  }

  void Fs::access(const string& path, AccessCb_t cb) {
    access(path, F_OK, cb);
  }

  void Fs::access(const string& path, int mode, AccessCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Access(path, mode, reqWrap);
  }

  void Fs::accessSync(const string& path, int mode /*= 0*/) {
    Access(path, mode);
  }

  Fs &fs = Fs::instance();
}