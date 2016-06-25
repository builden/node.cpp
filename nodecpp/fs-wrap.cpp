#include "fs-wrap.h"
#include "callback.h"
#include <uv.h>

namespace nodecpp {
  using ReadCb = Callback<ReadCb_t>;
  using WriteCb = Callback<WriteCb_t>;

  class FsWrap::impl {
  public:
    impl(int fd) : fd_(fd) {}

    void readFile(ReadCb_t cb) {
      iov_ = uv_buf_init(buffer_, sizeof(buffer_));
      req_.data = new ReadCb(cb, (void *)this);
      uv_fs_read(uv_default_loop(), &req_, fd_,
        &iov_, 1, -1, readCb);
    }

    void writeFile(const Buffer& buf, WriteCb_t cb) {
      buf_ = buf;
      auto iov = uv_buf_init(const_cast<char*>(buf_.data()), buf_.length());
      req_.data = new WriteCb(cb, this);
      uv_fs_write(uv_default_loop(), &req_, fd_, &iov, 1, 0, writeCb);
    }

  private:
    static void readCb(uv_fs_t* req) {
      auto innerCb = (ReadCb *)req->data;
      auto fsWrap = (impl *)innerCb->getData();
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
    static void writeCb(uv_fs_t* req) {
      auto innerCb = (WriteCb *)req->data;
      auto fsWrap = (impl *)innerCb->getData();

      if (req->result < 0) {
        innerCb->invoke(Error(req->result));
      }
      else {
        innerCb->invoke(Error(0));
      }
      uv_fs_req_cleanup(req);
    }

  private:
    uv_buf_t iov_;
    char buffer_[1024];
    int fd_;
    uv_fs_t req_;
    Buffer buf_;
  };

  FsWrap::FsWrap(int fd) : pimpl(new impl(fd)) {}

  FsWrap::~FsWrap() {}

  void FsWrap::readFile(ReadCb_t cb) {
    pimpl->readFile(cb);
  }

  void FsWrap::writeFile(const Buffer& buf, WriteCb_t cb) {
    pimpl->writeFile(buf, cb);
  }

}
