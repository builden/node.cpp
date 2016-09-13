#include "buffer.h"
#include "core-math.h"
#include "iconv.h"
#include "fmt/format.h"
#include "lodash.h"
#include "base64/base64.h"

namespace nodecpp {

  Buffer Buffer::alloc(uint32_t size, uint8_t fill /*= 0x00*/) {
    return Buffer(size, fill);
  }

  Buffer::Buffer() {

  }

  Buffer::Buffer(uint32_t size, uint8_t fill /*= 0x00*/) {
    buf_.resize(size);
    if (fill != 0x00) std::fill(buf_.begin(), buf_.end(), fill);
  }

  Buffer::Buffer(const char* buf, uint32_t size, uint32_t byteOffset /*= 0*/) {
    buf_.assign(buf + byteOffset, buf + size);
  }

  Buffer::Buffer(const string& str, const string& encoding /*= "ansi"*/) {
    string utf8str;
    if (encoding == "ansi") {
      utf8str = iconv.strToUtf8(str);
    }
    else if (encoding == "utf8" || encoding == "utf-8") {
      utf8str = str;
    }
    else if (encoding == "base64") {
      Base64Decode(str, &utf8str);
    }
    else if (encoding == "hex") {
      for (unsigned int i = 0; i < str.length(); i += 2) {
        string byteString = str.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        utf8str.push_back(byte);
      }
    }

    buf_.assign(utf8str.begin(), utf8str.end());
  }

  Buffer::Buffer(const wstring& wstr) {
    string utf8str = iconv.wstrToUtf8(wstr);
    buf_.assign(utf8str.begin(), utf8str.end());
  }

  Buffer::Buffer(const Buffer& buffer) {
    buf_ = buffer.buf_;
  }

  Buffer::Buffer(const cvec_t& cvec) {
    buf_ = cvec;
  }

  string Buffer::format(uint32_t num) const {
    fmt::MemoryWriter out;
    out << "<Buffer";
    uint32_t showNum = Math.min(num, buf_.size());
    for (uint32_t i = 0; i < showNum; i++) {
      out.write(" {:02x}", (uint8_t)buf_[i]);
    }
    if (showNum < buf_.size()) out << " ... ";
    if (showNum == 0) out << " ";
    out << ">";

    return out.str();
  }

  Buffer Buffer::from(const char* buf, uint32_t size, uint32_t byteOffset /*= 0*/) {
    return Buffer(buf, size, byteOffset);
  }


  Buffer Buffer::concat(vector<Buffer>& bufs, uint32_t len) {
    Buffer buffer(len);
    uint32_t pos = 0;
    for (size_t i = 0; i < bufs.size(); ++i) {
      Buffer& buf = bufs[i];
      buffer.write(buf, pos);
      pos += buf.size();
    }
    return buffer;
  }

  Buffer Buffer::concat(std::list<Buffer>& bufs, uint32_t len) {
    Buffer buffer(len);
    uint32_t pos = 0;
    for (auto it = bufs.begin(); it != bufs.end(); it++) {
      Buffer& buf = (*it);
      buffer.write(buf, pos);
      pos += buf.size();
    }
    return buffer;
  }

  bool Buffer::equals(const Buffer& buffer) {
    return buf_ == buffer.buf_;
  }

  string Buffer::toString(const string& encoding /*= "ansi"*/, uint32_t start /*= 0*/) const {
    uint32_t end = buf_.size();
    return toString(encoding, start, end);
  }

  string Buffer::toString(const string& encoding, uint32_t start, uint32_t end) const {
    const char* str = buf_.data();

    string rst;
    if (end > start && start < static_cast<int>(buf_.size())) rst = string(str + start, str + end);
    if (encoding == "ansi") {
      return iconv.utf8ToStr(rst);
    }
    else if (encoding == "base64") {
      string base64;
      Base64Encode(rst, &base64);
      return base64;
    }
    else if (encoding == "hex") {
      fmt::MemoryWriter out;
      for (auto& c : buf_) {
        out.write("{:02x}", (uint8_t)c);
      }
      rst = out.str();
    }
    return rst;
  }

  string Buffer::toU8string(uint32_t start /*= 0*/) const {
    uint32_t end = buf_.size();
    return toU8string(start, end);
  }

  string Buffer::toU8string(uint32_t start, uint32_t end) const {
    const char* str = buf_.data();

    string rst;
    if (end > start && start < static_cast<int>(buf_.size())) rst = string(str + start, str + end);
    return rst;
  }

  wstring Buffer::toWstring(uint32_t start /*= 0*/) const {
    uint32_t end = buf_.size();
    return toWstring(start, end);
  }

  wstring Buffer::toWstring(uint32_t start, uint32_t end) const {
    const char* str = buf_.data();

    string rst;
    if (end > start && start < static_cast<int>(buf_.size())) rst = string(str + start, str + end);
    return iconv.utf8ToWstr(rst);
  }

  const char* Buffer::data() const {
    return (const char*)&buf_[0];
  }

  uint32_t Buffer::length() const {
    return buf_.size();
  }


  char & Buffer::operator[](int i) {
    return buf_[i];
  }

  const char & Buffer::operator[](int i) const {
    return buf_[i];
  }

  bool Buffer::operator==(const Buffer& rhs) const {
    return rhs.buf_ == buf_;
  }

  void Buffer::append(const char* buf, uint32_t size) {
    buf_.insert(buf_.end(), buf, buf + size);
  }

  void Buffer::append(const Buffer& buffer) {
    buf_.insert(buf_.end(), buffer.buf_.begin(), buffer.buf_.end());
  }

  double Buffer::readDoubleBE(uint32_t /*offset*/ /*= 0*/) const {
    return 0;
  }

  double Buffer::readDoubleLE(uint32_t /*offset*/ /*= 0*/) const {
    return 0;
  }

  float Buffer::readFloatBE(uint32_t /*offset*/ /*= 0*/) const {
    return 0;
  }

  float Buffer::readFloatLE(uint32_t /*offset*/ /*= 0*/) const {
    return 0;
  }

  int8_t Buffer::readInt8(uint32_t offset /*= 0*/) const {
    if (offset + 1 > buf_.size()) return 0;
    return buf_[offset];
  }

  int16_t Buffer::readInt16BE(uint32_t offset /*= 0*/) const {
    if (offset + 2 > buf_.size()) return 0;
    int16_t rst = buf_[offset + 1] | (buf_[offset] << 8);
    return rst;
  }

  int16_t Buffer::readInt16LE(uint32_t offset /*= 0*/) const {
    if (offset + 2 > buf_.size()) return 0;
    int16_t rst = buf_[offset] | (buf_[offset + 1] << 8);
    return rst;
  }

  int32_t Buffer::readInt32BE(uint32_t offset /*= 0*/) const {
    if (offset + 4 > buf_.size()) return 0;
    int32_t rst = (buf_[offset] << 24) |
      (buf_[offset + 1] << 16) |
      (buf_[offset + 2] << 8) |
      (buf_[offset + 3]);
    return rst;
  }

  int32_t Buffer::readInt32LE(uint32_t offset /*= 0*/) const {
    if (offset + 4 > buf_.size()) return 0;
    int32_t rst = (buf_[offset]) |
      (buf_[offset + 1] << 8) |
      (buf_[offset + 2] << 16) |
      (buf_[offset + 3] << 24);
    return rst;
  }

  uint8_t Buffer::readUInt8(uint32_t offset /*= 0*/) const {
    if (offset >= buf_.size()) return 0;
    return buf_[offset];
  }

  uint16_t Buffer::readUInt16BE(uint32_t offset /*= 0*/) const {
    if (offset + 2 > buf_.size()) return 0;
    uint16_t rst = (buf_[offset] << 8) | buf_[offset + 1];
    return rst;
  }

  uint16_t Buffer::readUInt16LE(uint32_t offset /*= 0*/) const {
    if (offset + 2 > buf_.size()) return 0;
    uint16_t rst = buf_[offset] | (buf_[offset + 1] << 8);
    return rst;
  }

  uint32_t Buffer::readUInt32BE(uint32_t offset /*= 0*/) const {
    if (offset + 4 > buf_.size()) return 0;
    uint32_t rst = (buf_[offset] * 0x1000000) +
      (buf_[offset + 1] << 16) |
      (buf_[offset + 2] << 8) |
      (buf_[offset + 3]);
    return rst;
  }

  uint32_t Buffer::readUInt32LE(uint32_t offset /*= 0*/) const {
    if (offset + 4 > buf_.size()) return 0;
    uint32_t rst = (buf_[offset]) |
      (buf_[offset + 1] << 8) |
      (buf_[offset + 2] << 16) +
      (buf_[offset + 3] * 0x1000000);
    return rst;
  }

  uint32_t Buffer::writeDoubleBE(double /*value*/, uint32_t /*offset*/ /*= 0*/) {
    return 0;
  }

  uint32_t Buffer::writeDoubleLE(double /*value*/, uint32_t /*offset*/ /*= 0*/) {
    return 0;
  }

  uint32_t Buffer::writeFloatBE(float /*value*/, uint32_t /*offset*/ /*= 0*/) {
    return 0;
  }

  uint32_t Buffer::writeFloatLE(float /*value*/, uint32_t /*offset*/ /*= 0*/) {
    return 0;
  }

  uint32_t Buffer::writeInt8(int8_t value, uint32_t offset /*= 0*/) {
    if (offset + 1 > buf_.size()) return offset;

    buf_[offset] = value;
    return offset + 1;
  }

  uint32_t Buffer::writeInt16BE(int16_t value, uint32_t offset /*= 0*/) {
    if (offset + 2 > buf_.size()) return offset;

    buf_[offset] = (value >> 8);
    buf_[offset + 1] = char(value >> 0);
    return offset + 2;
  }

  uint32_t Buffer::writeInt16LE(int16_t value, uint32_t offset /*= 0*/) {
    if (offset + 2 > buf_.size()) return offset;

    buf_[offset] = char(value >> 0);
    buf_[offset + 1] = (value >> 8);
    return offset + 2;
  }

  uint32_t Buffer::writeInt32BE(int32_t value, uint32_t offset /*= 0*/) {
    if (offset + 4 > buf_.size()) return offset;

    buf_[offset] = (value >> 24);
    buf_[offset + 1] = char(value >> 16);
    buf_[offset + 2] = char(value >> 8);
    buf_[offset + 3] = char(value >> 0);
    return offset + 4;
  }

  uint32_t Buffer::writeInt32LE(int32_t value, uint32_t offset /*= 0*/) {
    if (offset + 4 > buf_.size()) return offset;

    buf_[offset] = char(value >> 0);
    buf_[offset + 1] = char(value >> 8);
    buf_[offset + 2] = char(value >> 16);
    buf_[offset + 3] = char(value >> 24);
    return offset + 4;
  }

  uint32_t Buffer::writeUInt8(uint8_t value, uint32_t offset /*= 0*/) {
    if (offset + 1 > buf_.size()) return offset;

    buf_[offset] = value;
    return offset + 1;
  }

  uint32_t Buffer::writeUInt16BE(uint16_t value, uint32_t offset /*= 0*/) {
    if (offset + 2 > buf_.size()) return offset;

    buf_[offset] = (value >> 8);
    buf_[offset + 1] = char(value >> 0);
    return offset + 2;
  }

  uint32_t Buffer::writeUInt16LE(uint16_t value, uint32_t offset /*= 0*/) {
    if (offset + 2 > buf_.size()) return offset;

    buf_[offset] = char(value >> 0);
    buf_[offset + 1] = (value >> 8);
    return offset + 2;
  }

  uint32_t Buffer::writeUInt32BE(uint32_t value, uint32_t offset /*= 0*/) {
    if (offset + 4 > buf_.size()) return offset;

    buf_[offset] = (value >> 24);
    buf_[offset + 1] = char(value >> 16);
    buf_[offset + 2] = char(value >> 8);
    buf_[offset + 3] = char(value);
    return offset + 4;
  }

  uint32_t Buffer::writeUInt32LE(uint32_t value, uint32_t offset /*= 0*/) {
    if (offset + 4 > buf_.size()) return offset;

    buf_[offset + 3] = (value >> 24);
    buf_[offset + 2] = char(value >> 16);
    buf_[offset + 1] = char(value >> 8);
    buf_[offset] = char(value);
    return offset + 4;
  }

  uint32_t Buffer::write(const Buffer& buf, uint32_t offset /*= 0*/) {
    if (offset >= buf_.size()) return 0;

    uint32_t remaining = buf_.size() - offset;
    uint32_t length = buf.length();
    remaining = Math.min(remaining, length);
    for (uint32_t i = 0; i < remaining; ++i) {
      buf_[offset + i] = buf[i];
    }
    return remaining;
  }

  nodecpp::Buffer Buffer::slice(int start /*= 0*/) const {
    int end = buf_.size();
    return slice(start, end);
  }

  Buffer Buffer::slice(int start, int end) const {
    return Buffer(_.slice(buf_, start, end));
  }


  Buffer Buffer::swap16() {
    return Buffer();
  }

  Buffer Buffer::swap32() {
    return Buffer();
  }

  uint32_t Buffer::size() const {
    return buf_.size();
  }

}
