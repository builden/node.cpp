/**
 * encoding: nodejs默认使用utf-8编码，buffer内部同样是utf-8编码
 *   "ansi"
 *   "utf-8" / "utf8"
 *   "hex"
 *   "base64"
 *   "utf16le" / "ucs2" / "utf-16le" / "ucs-2"
 *
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__
#include "nodecpp-def.h"

namespace nodecpp {

  class Buffer {
  public:
    Buffer();
    Buffer(uint32_t size);
    Buffer(const char* buf, uint32_t size, uint32_t byteOffset = 0);
    Buffer(const string& str, const string& encoding = "ansi");
    Buffer(const wstring& wstr);
    Buffer(const Buffer& buffer);
    Buffer(const cvec_t& cvec);

    void append(const char* buf, uint32_t size);
    void append(const Buffer& buffer);

    // length < 0, length <Number> Default: buf.length - byteOffset
    static Buffer from(const char* buf, uint32_t size, uint32_t byteOffset = 0);

    bool equals(const Buffer& buffer);

    // encoding
    //  ansi / utf8 / base64 / hex
    string toString(const string& encoding = "ansi", uint32_t start = 0) const;
    string toString(const string& encoding, uint32_t start, uint32_t end) const;
    wstring toWstring(uint32_t start = 0) const;
    wstring toWstring(uint32_t start, uint32_t end) const;

    string format(uint32_t num = 50) const;
    const char* data() const;
    uint32_t length() const;
    uint32_t size() const;

    char & operator[](int i);
    const char & operator[](int i) const;
    bool operator==(const Buffer& rhs) const;

    double readDoubleBE(uint32_t offset = 0) const;
    double readDoubleLE(uint32_t offset = 0) const;
    float readFloatBE(uint32_t offset = 0) const;
    float readFloatLE(uint32_t offset = 0) const;
    int8_t readInt8(uint32_t offset = 0) const;
    int16_t readInt16BE(uint32_t offset = 0) const;
    int16_t readInt16LE(uint32_t offset = 0) const;
    int32_t readInt32BE(uint32_t offset = 0) const;
    int32_t readInt32LE(uint32_t offset = 0) const;
    uint8_t readUInt8(uint32_t offset = 0) const;
    uint16_t readUInt16BE(uint32_t offset = 0) const;
    uint16_t readUInt16LE(uint32_t offset = 0) const;
    uint32_t readUInt32BE(uint32_t offset = 0) const;
    uint32_t readUInt32LE(uint32_t offset = 0) const;

    uint32_t writeDoubleBE(double value, uint32_t offset = 0);
    uint32_t writeDoubleLE(double value, uint32_t offset = 0);
    uint32_t writeFloatBE(float value, uint32_t offset = 0);
    uint32_t writeFloatLE(float value, uint32_t offset = 0);
    uint32_t writeInt8(int8_t value, uint32_t offset = 0);
    uint32_t writeInt16BE(int16_t value, uint32_t offset = 0);
    uint32_t writeInt16LE(int16_t value, uint32_t offset = 0);
    uint32_t writeInt32BE(int32_t value, uint32_t offset = 0);
    uint32_t writeInt32LE(int32_t value, uint32_t offset = 0);
    uint32_t writeUInt8(uint8_t value, uint32_t offset = 0);
    uint32_t writeUInt16BE(uint16_t value, uint32_t offset = 0);
    uint32_t writeUInt16LE(uint16_t value, uint32_t offset = 0);
    uint32_t writeUInt32BE(uint32_t value, uint32_t offset = 0);
    uint32_t writeUInt32LE(uint32_t value, uint32_t offset = 0);

    Buffer slice(int start = 0);
    Buffer slice(int start, int end);

    Buffer swap16();
    Buffer swap32();

  private:
    cvec_t buf_;
  };
}

#endif // !__BUFFER_H__
