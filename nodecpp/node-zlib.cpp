#include "node-zlib.h"

#include <zlib.h>
#include <uv.h>
#pragma comment(lib, "zlib")

namespace nodecpp {

  enum ZlibMode {
    NONE,
    DEFLATE,
    INFLATE,
    GZIP,
    GUNZIP,
    DEFLATERAW,
    INFLATERAW,
    UNZIP
  };

  class Zlib::impl {
  public:
    Buffer zlibDeflate(ZlibMode mode, const Buffer& buf) {
      z_stream zs;                        // z_stream is zlib's control structure
      memset(&zs, 0, sizeof(zs));

      int windowBits = Z_DEFAULT_WINDOWBITS;
      if (mode == GZIP) windowBits += 16;
      if (mode == DEFLATERAW) windowBits *= -1;

      if (deflateInit2(&zs, Z_DEFAULT_LEVEL, Z_DEFLATED, windowBits, Z_DEFAULT_MEMLEVEL, Z_DEFAULT_STRATEGY) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));

      zs.next_in = (Bytef*)buf.data();
      zs.avail_in = buf.size();           // set the z_stream's input

      int ret = Z_OK;
      char outbuffer[16384] = { 0 };
      Buffer rst;

      // retrieve the compressed bytes blockwise
      do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (rst.size() < zs.total_out) {
          // append the block to the output string
          rst.append(outbuffer,
            zs.total_out - rst.size());
        }
      } while (ret == Z_OK);

      deflateEnd(&zs);

      if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
      }

      return rst;
    }

    Buffer zlibInflate(ZlibMode mode, const Buffer& buf) {
      z_stream zs;                        // z_stream is zlib's control structure
      memset(&zs, 0, sizeof(zs));

      int windowBits = Z_DEFAULT_WINDOWBITS;
      if (mode == GUNZIP) windowBits += 16;
      else if (mode == INFLATERAW) windowBits *= -1;
      else if (mode == UNZIP) windowBits += 32;

      if (inflateInit2(&zs, windowBits) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

      zs.next_in = (Bytef*)buf.data();
      zs.avail_in = buf.size();

      int ret;
      char outbuffer[32768];
      Buffer rst;

      // get the decompressed bytes blockwise using repeated calls to inflate
      do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (rst.size() < zs.total_out) {
          rst.append(outbuffer,
            zs.total_out - rst.size());
        }

      } while (ret == Z_OK);

      inflateEnd(&zs);

      if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
          << zs.msg;
        throw(std::runtime_error(oss.str()));
      }

      return rst;
    }
  };

  Zlib::Zlib() : pimpl(new impl) {}
  Zlib::~Zlib() {}

  Zlib& zlib = Zlib::instance();

  Buffer Zlib::deflateSync(const Buffer& buf) {
    return pimpl->zlibDeflate(DEFLATE, buf);
  }

  Buffer Zlib::inflateSync(const Buffer& buf) {
    return pimpl->zlibInflate(INFLATE, buf);
  }

  Buffer Zlib::deflateRawSync(const Buffer& buf) {
    return pimpl->zlibDeflate(DEFLATERAW, buf);
  }

  Buffer Zlib::inflateRawSync(const Buffer& buf) {
    return pimpl->zlibInflate(INFLATERAW, buf);
  }

  Buffer Zlib::gzipSync(const Buffer& buf) {
    return pimpl->zlibDeflate(GZIP, buf);
  }

  Buffer Zlib::gunzipSync(const Buffer& buf) {
    return pimpl->zlibInflate(GUNZIP, buf);
  }

  Buffer Zlib::unzipSync(const Buffer& buf) {
    return pimpl->zlibInflate(UNZIP, buf);
  }

}