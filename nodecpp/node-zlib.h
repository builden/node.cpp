#pragma once
#include "nodecpp-def.h"
#include "singleton.h"
#include "buffer.h"

namespace nodecpp {
  const int Z_NO_FLUSH = 0;
  const int Z_PARTIAL_FLUSH = 1;
  const int Z_SYNC_FLUSH = 2;
  const int Z_FULL_FLUSH = 3;
  const int Z_FINISH = 4;
  const int Z_BLOCK = 5;
  const int Z_TREES = 6;

  const int Z_DEFAULT_WINDOWBITS = 15;
  const int Z_MIN_WINDOWBITS = 8;
  const int Z_MAX_WINDOWBITS = 15;

  const int Z_DEFAULT_COMPRESSION = -1;
  const int Z_DEFAULT_LEVEL = Z_DEFAULT_COMPRESSION;
  const int Z_MIN_LEVEL = -1;
  const int Z_MAX_LEVEL = 9;

  const int Z_DEFAULT_MEMLEVEL = 8;
  const int Z_MIN_MEMLEVEL = 1;
  const int Z_MAX_MEMLEVEL = 9;

  const int Z_DEFAULT_STRATEGY = 0;

  class ZlibOptions {
  public:
    int flush = Z_NO_FLUSH;
    int finishFlush = Z_FINISH;
    int chunkSize = 16 * 1024;
    int windowBits = Z_DEFAULT_WINDOWBITS;
    int level = Z_DEFAULT_LEVEL;        // compression only
    int memLevel = Z_DEFAULT_MEMLEVEL;  // compression only
    int strategy = Z_DEFAULT_STRATEGY;
    Buffer dictionary;
  };

  class Zlib : public Singleton<Zlib> {
  public:
    Zlib();
    ~Zlib();

  public:
    Buffer deflateSync(const Buffer& buf);
    Buffer inflateSync(const Buffer& buf);

    Buffer deflateRawSync(const Buffer& buf);
    Buffer inflateRawSync(const Buffer& buf);

    Buffer gzipSync(const Buffer& buf);
    Buffer gunzipSync(const Buffer& buf);

    // 通过自动检测头解压缩一个 Gzip- 或 Deflate- 压缩的数据，不包含DeflateRaw
    Buffer unzipSync(const Buffer& buf);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern Zlib& zlib;
}