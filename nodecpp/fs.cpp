#include "fs.h"
#include "iconv.h"
#include "fmt/format.h"
#include "path.h"

#include <uv.h>
#include <fcntl.h>
#include "fs-req-wrap.h"

namespace nodecpp {
  void Fs::open(const string& p, const string& flags, OpenCb_t cb) {
    open(p, flags, 0, cb);
  }

  void Fs::open(const string& p, const string& flags, int mode, OpenCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteResult = cb;
    Open(iconv.strToUtf8(p), stringToFlags(flags), mode, reqWrap);
  }

  int Fs::openSync(const string& p, const string& flags, int mode /*= 0*/) {
    return Open(iconv.strToUtf8(p), stringToFlags(flags), mode);
  }

  void Fs::close(int fd, CloseCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Close(fd, reqWrap);
  }

  void Fs::closeSync(int fd) {
    Close(fd);
  }

  void Fs::readFile(const string& p, ReadCb_t cb) {
    // flags default 'r'
    auto context = new ReadFileContext(cb);
    context->isUserFd_ = false;
    auto req = FSReqWrap::New(nullptr);
    req->context = context;
    req->onCompleteResult = [context](const Error& err, int fd) {
      context->readFileAfterOpen(err, fd);
    };
    Open(iconv.strToUtf8(p), stringToFlags("r"), 0666, req);
  }

  void Fs::readFile(const string& p, const string& encoding, ReadStrCb_t cb) {
    readFile(p, [&](const Error& err, const Buffer& buf) {
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


  uint32_t Fs::tryReadSync(int fd, Buffer& buffer, uint32_t pos, uint32_t len) {
    uint32_t bytesRead = 0;
    __try {
      bytesRead = readSync(fd, buffer, pos, len);
    }
    __finally {

    }
    return bytesRead;
  }

  void Fs::writeAll(int fd, bool isUserFd, const Buffer& buffer, uint32_t offset, uint32_t length, int64_t position, WriteCb_t cb) {
    write(fd, buffer, offset, length, position, [=](const Error& err, uint32_t written) {
      if (err) {
        if (isUserFd) {
          cb(err);
        }
        else {
          this->close(fd, [=](const Error&) {
            cb(err);
          });
        }
      }
      else {
        if (written == length) {
          if (isUserFd) {
            cb(err);
          }
          else {
            this->close(fd, cb);
          }
        }
        else {
          auto off = offset + written;
          auto len = length - written;
          auto pos = position;
          if (position != -1) {
            pos = position + written;
          }
          this->writeAll(fd, isUserFd, buffer, off, len, pos, cb);
        }
      }
    });
  }

  Buffer Fs::readFileSync(const string& p) {
    int fd = openSync(iconv.strToUtf8(p), "r", 0666);
    Stats st = fstatSync(fd);
    uint32_t size = st.isFile() ? static_cast<uint32_t>(st.size) : 0;
    Buffer buffer;
    vector<Buffer> buffers;
    uint32_t pos = 0;
    if (size != 0) {
      buffer = Buffer(size);
    }
    uint32_t bytesRead = 0;
    if (size != 0) {
      do {
        bytesRead = tryReadSync(fd, buffer, pos, size - pos);
        pos += bytesRead;
      } while (bytesRead != 0 && pos < size);
    }
    else {
      do {
        // the kernel lies about many files.
        // Go ahead and try to read some bytes.
        buffer = Buffer(8192);
        bytesRead = tryReadSync(fd, buffer, 0, 8192);
        if (bytesRead != 0) {
          buffers.emplace_back(buffer.slice(0, bytesRead));
        }
        pos += bytesRead;
      } while (bytesRead != 0);
    }
    closeSync(fd);

    if (size == 0) {
      buffer = Buffer::concat(buffers, pos);
    }
    else if (pos < size) {
      buffer = buffer.slice(0, pos);
    }
    return buffer;
  }

  string Fs::readFileSync(const string& p, const string& encoding) {
    Buffer buf = readFileSync(p);
    return buf.toString(encoding);
  }

  void Fs::writeFile(const string& p, const Buffer& data, WriteCb_t cb) {
    open(p, "w", 0666, [=](const Error& openErr, int fd) {
      if (openErr) {
        cb(openErr);
      }
      else {
        writeAll(fd, false, data, 0, data.size(), 0, cb);
      }
    });
  }

  void Fs::writeFileSync(const string& p, const Buffer& data) {
    int fd = openSync(p, "w", 0666);
    uint32_t offset = 0;
    uint32_t length = data.size();
    uint32_t position = 0;
    while (length > 0) {
      uint32_t written = fs.writeSync(fd, data, offset, length, position);
      offset += written;
      length -= written;
      if (position != -1) {
        position += written;
      }
    }
    closeSync(fd);
  }

  void Fs::stat(const string& p, StatCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteStats = cb;
    Stat(iconv.strToUtf8(p), reqWrap);
  }

  Stats Fs::statSync(const string& p) {
    return Stat(iconv.strToUtf8(p));
  }

  void Fs::lstat(const string& p, StatCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteStats = cb;
    LStat(iconv.strToUtf8(p), reqWrap);
  }

  Stats Fs::lstatSync(const string& p) {
    return LStat(iconv.strToUtf8(p));
  }

  void Fs::fstat(int fd, StatCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteStats = cb;
    FStat(fd, reqWrap);
  }

  Stats Fs::fstatSync(int fd) {
    return FStat(fd);
  }

  void Fs::exists(const string& p, ExistsCb_t cb) {
    stat(p, [cb](const Error& err, const Stats&) {
      cb(!err);
    });
  }

  bool Fs::existsSync(const string& p) {
    try {
      statSync(p);
      return true;
    }
    catch (const Error&) {
      return false;
    }
  }

  void Fs::mkdir(const string& p, MkdirCb_t cb) {
    mkdir(p, 0777, cb);
  }

  void Fs::mkdir(const string& p, int mode, MkdirCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    MKDir(iconv.strToUtf8(p), mode, reqWrap);
  }

  void Fs::mkdirSync(const string& p, int mode /*= 0777*/) {
    MKDir(iconv.strToUtf8(p), mode);
  }

  void Fs::mkdirs(const string& p, MkdirCb_t cb) {
    auto _p = path.resolve(p);
    mkdir(_p, [_p, cb](const Error& err) {
      if (!err) {
        cb(err);
        return;
      }
      switch (err.code()) {
      case -4058:
      {
        if (path.dirname(_p) == _p) throw err;
        fs.mkdirs(path.dirname(_p), [_p, cb](const Error& er) {
          if (er) cb(er);
          else fs.mkdirs(_p, cb);
        });
        break;
      }
      default:
        int errCode = err.code();
        fs.stat(_p, [cb, errCode](const Error& er2, const Stats& stats) {
          if (er2 || !stats.isDirectory()) cb(Error(errCode));
          else cb(Error());
        });
        break;
      }
    });
  }

  void Fs::mkdirsSync(const string& p) {
    auto _p = path.resolve(p);
    try {
      mkdirSync(_p);
    }
    catch (const Error& err) {
      switch (err.code()) {
      case -4058: // no such file or directory
      {
        if (path.dirname(_p) == _p) throw err;
        mkdirsSync(path.dirname(_p));
        mkdirsSync(_p);
        break;
      }
      default:
        Stats stats;
        try {
          stats = statSync(_p);
        }
        catch (const Error& err) {
          throw err;
        }
        if (!stats.isDirectory()) throw err;
        break;
      }
    }
  }

  void Fs::rmdir(const string& p, RmdirCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    RMDir(iconv.strToUtf8(p), reqWrap);
  }

  void Fs::rmdirSync(const string& p) {
    RMDir(iconv.strToUtf8(p));
  }

  void Fs::unlink(const string& p, UnlinkCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Unlink(iconv.strToUtf8(p), reqWrap);
  }

  void Fs::unlinkSync(const string& p) {
    Unlink(iconv.strToUtf8(p));
  }

  void Fs::rename(const string& oldPath, const string& newPath, RenameCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Rename(iconv.strToUtf8(oldPath), iconv.strToUtf8(newPath), reqWrap);
  }

  void Fs::renameSync(const string& oldPath, const string& newPath) {
    Rename(iconv.strToUtf8(oldPath), iconv.strToUtf8(newPath));
  }

  void Fs::access(const string& p, AccessCb_t cb) {
    access(p, F_OK, cb);
  }

  void Fs::access(const string& p, int mode, AccessCb_t cb) {
    FSReqWrap* reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onComplete = cb;
    Access(iconv.strToUtf8(p), mode, reqWrap);
  }

  void Fs::accessSync(const string& p, int mode /*= 0*/) {
    Access(iconv.strToUtf8(p), mode);
  }


  uint32_t Fs::readSync(int fd, Buffer& buffer, uint32_t offset, uint32_t length, int64_t position /*= -1*/) {
    return Read(fd, buffer, offset, length, position);
  }

  Fs &fs = Fs::instance();

  void Fs::write(int fd, const Buffer& buffer, uint32_t offset, uint32_t length, int64_t position, WriteAsyncCb_t cb) {
    auto reqWrap = FSReqWrap::New(nullptr);
    reqWrap->onCompleteResult = cb;
    reqWrap->writeBuffer = buffer;
    WriteBuffer(fd, reqWrap->writeBuffer, offset, length, position, reqWrap);
  }

  uint32_t Fs::writeSync(int fd, const Buffer& buffer, uint32_t offset, uint32_t length, int64_t position /*= -1*/) {
    return WriteBuffer(fd, buffer, offset, length, position);
  }

  json Fs::readJsonSync(const string& file) {
    return json::parse(readFileSync(file).toString());
  }

}