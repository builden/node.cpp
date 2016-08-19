#pragma once
#include "async-wrap.h"
#include "buffer.h"
#include <zlib.h>
#include <uv.h>

namespace nodecpp {
  enum node_zlib_mode {
    NONE,
    DEFLATE,
    INFLATE,
    GZIP,
    GUNZIP,
    DEFLATERAW,
    INFLATERAW,
    UNZIP
  };

  class ZCtx : public AsyncWrap {
  public:
    // onmessage(message, code)
    using ErrorCb_t = function<void(const string&, int)>;
    using WriteCb_t = function<void(uint32_t, uint32_t)>;

    ZCtx(node_zlib_mode mode);
    ~ZCtx();

    void Close();
    void Write(uint32_t flush, const Buffer& in_buf, size_t in_len, size_t in_off, Buffer& out_buf, size_t out_len, size_t out_off, bool async = true);

    static void Process(uv_work_t* work_req);
    static void Reset(ZCtx* ctx);
    static void Error(ZCtx* ctx, const char* message);
    static bool CheckError(ZCtx* ctx);
    static void After(uv_work_t* work_req, int status);
    static void AfterSync(ZCtx* ctx);

    ErrorCb_t errorCb_ = nullptr;
    WriteCb_t writeCb_ = nullptr;
  private:
    Bytef* dictionary_;
    size_t dictionary_len_;
    int err_;
    int flush_;
    bool init_done_;
    int level_;
    int memLevel_;
    node_zlib_mode mode_;
    int strategy_;
    z_stream strm_;
    int windowBits_;
    uv_work_t work_req_;
    bool write_in_progress_;
    bool pending_close_;
    unsigned int refs_;
    unsigned int gzip_id_bytes_read_;
  };
}