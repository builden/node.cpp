#include "read-file-context.h"
#include "fs.h"
#include "fs-req-wrap.h"
#include "process.h"

namespace nodecpp {

  ReadFileContext::ReadFileContext(CtxCb_t cb)
    : callback_(cb) {

  }

  void ReadFileContext::read() {
    uint32_t offset = 0;
    uint32_t length = 0;
    if (size_ == 0) {
      buffer_ = Buffer(kReadFileBufferLength);
      length = kReadFileBufferLength;
    }
    else {
      offset = pos_;
      length = size_ - pos_;
    }

    auto req = FSReqWrap::New(nullptr);
    req->onCompleteResult = [this](const Error& err, int bytesRead) {
      this->readFileAfterRead(err, bytesRead);
    };
    req->context = this;

    Read(fd_, buffer_, offset, length, -1, req);
  }

  void ReadFileContext::close(const Error& err) {
    auto req = FSReqWrap::New(nullptr);
    req->onComplete = [this](const Error& err) {
      this->readFileAfterClose(err);
    };
    req->context = this;
    this->err_ = err;

    if (this->isUserFd_) {
      process.nextTick([&]() {
        req->onComplete(Error(0));
      });
      return;
    }

    Close(fd_, req);
  }

  void ReadFileContext::readFileAfterOpen(const Error& err, int fd) {
    if (err) {
      callback_(err, Buffer());
    }

    fd_ = fd;
    auto req = FSReqWrap::New(nullptr);
    req->onCompleteStats = [this](const Error& err, const Stats& st) {
      this->readFileAfterStat(err, st);
    };
    req->context = this;
    FStat(fd, req);
  }

  void ReadFileContext::readFileAfterStat(const Error& err, const Stats& st) {
    if (err) {
      close(err);
      return;
    }

    size_ = st.isFile() ? static_cast<uint32_t>(st.size) : 0;
    if (size_ == 0) {
      read();
      return;
    }

    if (size_ > Buffer::kMaxLength) {
      close(Error("File size is greater than possible Buffer"));
      return;
    }

    buffer_ = Buffer(size_);
    read();
  }

  void ReadFileContext::readFileAfterRead(const Error& err, int bytesRead) {
    if (err) {
      close(err);
      return;
    }

    if (bytesRead == 0) {
      close(Error("readFileAfterRead bytesRead is zero"));
      return;
    }

    pos_ += bytesRead;

    if (size_ != 0) {
      if (pos_ == size_)
        close(Error("readFileAfterRead pos_ == size_"));
      else
        read();
    }
    else {
      // unknown size, just read until we don't get bytes.
      buffers_.emplace_back(buffer_.slice(0, bytesRead));
      read();
    }
  }

  void ReadFileContext::readFileAfterClose(const Error& err) {
    Buffer buffer;
    if (err) {
      callback_(err, buffer);
    }

    if (size_ == 0) {
      buffer = Buffer::concat(buffers_, pos_);
    }
    else if (pos_ < size_) {
      buffer = buffer_.slice(0, pos_);
    }
    else {
      buffer = buffer_;
    }

    callback_(err, buffer);
  }

}

