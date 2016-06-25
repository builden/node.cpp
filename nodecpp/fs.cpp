#include "fs.h"
#include "iconv.h"
#include "callback.h"
#include "fmt/format.h"

#include <fcntl.h>

namespace nodecpp {
  using OpenCb = Callback<OpenCb_t>;
  using ReadCb = Callback<ReadCb_t>;
  using WriteCb = Callback<WriteCb_t>;

  void Fs::open(const string& path, const string& flags, OpenCb_t cb) {
    auto openReq = new uv_fs_t;
    openReq->data = new OpenCb(cb);

    uv_fs_open(uv_default_loop(), openReq, iconv.strToUtf8(path).c_str(), stringToFlags(flags), 0, [](uv_fs_t* req) {
      auto cb = (OpenCb *)req->data;
      cb->invoke(Error(req->result), req->result);

      uv_fs_req_cleanup(req);
      delete cb;
      delete req;
    });
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

  void Fs::stat(const string& file, StatCb_t cb) {

  }

  Fs::Stats Fs::statSync(const string& path) {
    string filePath = iconv.strToUtf8(path);
    uv_fs_t req;
    int rc = uv_fs_stat(uv_default_loop(), &req, filePath.c_str(), nullptr);

    Stats stats;
    if (rc == 0) {
      uv_stat_t* const s = static_cast<uv_stat_t*>(req.ptr);
      rc = !!(s->st_mode & S_IFDIR);
      
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
    }
    else {
      uv_fs_req_cleanup(&req);
      throw Error(rc);
    }
    uv_fs_req_cleanup(&req);
    return stats;
  }

  void Fs::exists(const string& path, ExistsCb_t cb) {

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

  }

  void Fs::mkdirSync(const string& path) {
    uv_fs_t req;
    int rc = uv_fs_mkdir(uv_default_loop(), &req, iconv.strToUtf8(path).c_str(), 0777, nullptr);
    uv_fs_req_cleanup(&req);

    if (rc != 0) throw Error(rc);
  }

  void Fs::rmdir(const string& path, RmdirCb_t cb) {

  }

  void Fs::rmdirSync(const string& path) {
    uv_fs_t req;
    int rc = uv_fs_rmdir(uv_default_loop(), &req, iconv.strToUtf8(path).c_str(), nullptr);
    uv_fs_req_cleanup(&req);

    if (rc != 0) throw Error(rc);
  }

  void Fs::unlink(const string& path, UnlinkCb_t cb) {

  }

  void Fs::unlinkSync(const string& path) {
    uv_fs_t req;
    int rc = uv_fs_unlink(uv_default_loop(), &req, iconv.strToUtf8(path).c_str(), nullptr);
    uv_fs_req_cleanup(&req);

    if (rc != 0) throw Error(rc);
  }

  void Fs::rename(const string& oldPath, const string& newPath, RenameCb_t cb) {

  }

  void Fs::renameSync(const string& oldPath, const string& newPath) {
    uv_fs_t req;
    int rc = uv_fs_rename(uv_default_loop(), &req, iconv.strToUtf8(oldPath).c_str(), iconv.strToUtf8(newPath).c_str(), nullptr);
    uv_fs_req_cleanup(&req);

    if (rc != 0) throw Error(rc);
  }

  Fs &fs = Fs::instance();

  void FsWrap::readFile(ReadCb_t cb) {
    iov_ = uv_buf_init(buffer_, sizeof(buffer_));
    req_.data = new ReadCb(cb, (void *)this);
    uv_fs_read(uv_default_loop(), &req_, fd_,
      &iov_, 1, -1, readCb);
  }

  void FsWrap::readCb(uv_fs_t* req) {
    auto innerCb = (ReadCb *)req->data;
    auto fsWrap = (FsWrap *)innerCb->getData();
    if (req->result < 0) {
      innerCb->invoke(Error(req->result), fsWrap->buf_);
      uv_fs_req_cleanup(req);
    }
    else if (req->result == 0) {
      innerCb->invoke(Error(0), fsWrap->buf_);
      delete innerCb;
      delete fsWrap;
      uv_fs_req_cleanup(req);
    }
    else {
      fsWrap->buf_.append(fsWrap->iov_.base, req->result);
      uv_fs_read(uv_default_loop(), &fsWrap->req_, fsWrap->fd_,
        &fsWrap->iov_, 1, -1, readCb);
    }
  }

  void FsWrap::writeFile(const Buffer& buf, WriteCb_t cb) {
    buf_ = buf;
    auto iov = uv_buf_init(const_cast<char*>(buf_.data()), buf_.length());
    req_.data = new WriteCb(cb, this);
    uv_fs_write(uv_default_loop(), &req_, fd_, &iov, 1, 0, writeCb);
  }

  void FsWrap::writeCb(uv_fs_t* req) {
    auto innerCb = (WriteCb *)req->data;
    auto fsWrap = (FsWrap *)innerCb->getData();

    if (req->result < 0) {
      innerCb->invoke(Error(req->result));
    }
    else {
      innerCb->invoke(Error(0));
    }
    uv_fs_req_cleanup(req);
  }

}