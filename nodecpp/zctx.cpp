#include "zctx.h"
#include "util.h"
#pragma comment(lib, "zlib")

namespace nodecpp {
#define GZIP_HEADER_ID1 0x1f
#define GZIP_HEADER_ID2 0x8b

  ZCtx::ZCtx(node_zlib_mode mode) : 
    dictionary_(nullptr),
    dictionary_len_(0),
    err_(0),
    flush_(0),
    init_done_(false),
    level_(0),
    memLevel_(0),
    mode_(mode),
    strategy_(0),
    windowBits_(0),
    write_in_progress_(false),
    pending_close_(false),
    refs_(0),
    gzip_id_bytes_read_(0),
    AsyncWrap(AsyncWrap::PROVIDER_ZLIB) {

  }

  ZCtx::~ZCtx() {
    CHECK_EQ(false, write_in_progress_ && "write in progress");
    Close();
  }

  void ZCtx::Close() {
    if (write_in_progress_) {
      pending_close_ = true;
      return;
    }

    pending_close_ = false;
    CHECK(init_done_ && "close before init");
    CHECK_LE(mode_, UNZIP);

    if (mode_ == DEFLATE || mode_ == GZIP || mode_ == DEFLATERAW) {
      (void)deflateEnd(&strm_);
    }
    else if (mode_ == INFLATE || mode_ == GUNZIP || mode_ == INFLATERAW ||
      mode_ == UNZIP) {
      (void)inflateEnd(&strm_);
    }
    mode_ = NONE;

    if (dictionary_ != nullptr) {
      delete[] dictionary_;
      dictionary_ = nullptr;
    }
  }

  void ZCtx::Write(uint32_t flush, const Buffer& in_buf, size_t in_len, size_t in_off, Buffer& out_buf, size_t out_len, size_t out_off, bool async /*= true*/) {
    CHECK(init_done_ && "write before init");
    CHECK(mode_ != NONE && "already finalized");

    CHECK_EQ(false, write_in_progress_ && "write already in progress");
    CHECK_EQ(false, pending_close_ && "close is pending");
    write_in_progress_ = true;

    if (flush != Z_NO_FLUSH &&
      flush != Z_PARTIAL_FLUSH &&
      flush != Z_SYNC_FLUSH &&
      flush != Z_FULL_FLUSH &&
      flush != Z_FINISH &&
      flush != Z_BLOCK) {
      CHECK(0 && "Invalid flush value");
    }

    Bytef *in = reinterpret_cast<Bytef *>((char*)in_buf.data() + in_off);
    Bytef *out = reinterpret_cast<Bytef *>((char*)out_buf.data() + out_off);

    // build up the work request
    uv_work_t* work_req = &(work_req_);

    strm_.avail_in = in_len;
    strm_.next_in = in;
    strm_.avail_out = out_len;
    strm_.next_out = out;
    flush_ = flush;

    if (!async) {
      // sync version
      Process(work_req);
      if (CheckError(this))
        AfterSync(this);
      return;
    }

    // async version
    uv_queue_work(uv_default_loop(),
      work_req,
      ZCtx::Process,
      ZCtx::After);
  }

  void ZCtx::Process(uv_work_t* work_req) {
    ZCtx *ctx = ContainerOf(&ZCtx::work_req_, work_req);

    const Bytef* next_expected_header_byte = nullptr;

    // If the avail_out is left at 0, then it means that it ran out
    // of room.  If there was avail_out left over, then it means
    // that all of the input was consumed.
    switch (ctx->mode_) {
    case DEFLATE:
    case GZIP:
    case DEFLATERAW:
      ctx->err_ = deflate(&ctx->strm_, ctx->flush_);
      break;
    case UNZIP:
      if (ctx->strm_.avail_in > 0) {
        next_expected_header_byte = ctx->strm_.next_in;
      }

      switch (ctx->gzip_id_bytes_read_) {
      case 0:
        if (next_expected_header_byte == nullptr) {
          break;
        }

        if (*next_expected_header_byte == GZIP_HEADER_ID1) {
          ctx->gzip_id_bytes_read_ = 1;
          next_expected_header_byte++;

          if (ctx->strm_.avail_in == 1) {
            // The only available byte was already read.
            break;
          }
        }
        else {
          ctx->mode_ = INFLATE;
          break;
        }

        // fallthrough
      case 1:
        if (next_expected_header_byte == nullptr) {
          break;
        }

        if (*next_expected_header_byte == GZIP_HEADER_ID2) {
          ctx->gzip_id_bytes_read_ = 2;
          ctx->mode_ = GUNZIP;
        }
        else {
          // There is no actual difference between INFLATE and INFLATERAW
          // (after initialization).
          ctx->mode_ = INFLATE;
        }

        break;
      default:
        CHECK(0 && "invalid number of gzip magic number bytes read");
      }

      // fallthrough
    case INFLATE:
    case GUNZIP:
    case INFLATERAW:
      ctx->err_ = inflate(&ctx->strm_, ctx->flush_);

      // If data was encoded with dictionary
      if (ctx->err_ == Z_NEED_DICT && ctx->dictionary_ != nullptr) {
        // Load it
        ctx->err_ = inflateSetDictionary(&ctx->strm_,
          ctx->dictionary_,
          ctx->dictionary_len_);
        if (ctx->err_ == Z_OK) {
          // And try to decode again
          ctx->err_ = inflate(&ctx->strm_, ctx->flush_);
        }
        else if (ctx->err_ == Z_DATA_ERROR) {
          // Both inflateSetDictionary() and inflate() return Z_DATA_ERROR.
          // Make it possible for After() to tell a bad dictionary from bad
          // input.
          ctx->err_ = Z_NEED_DICT;
        }
      }

      while (ctx->strm_.avail_in > 0 &&
        ctx->mode_ == GUNZIP &&
        ctx->err_ == Z_STREAM_END &&
        ctx->strm_.next_in[0] != 0x00) {
        // Bytes remain in input buffer. Perhaps this is another compressed
        // member in the same archive, or just trailing garbage.
        // Trailing zero bytes are okay, though, since they are frequently
        // used for padding.

        Reset(ctx);
        ctx->err_ = inflate(&ctx->strm_, ctx->flush_);
      }
      break;
    default:
      CHECK(0 && "wtf?");
    }

    // pass any errors back to the main thread to deal with.

    // now After will emit the output, and
    // either schedule another call to Process,
    // or shift the queue and call Process.
  }

  void ZCtx::Reset(ZCtx* ctx) {
    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
    case DEFLATE:
    case DEFLATERAW:
    case GZIP:
      ctx->err_ = deflateReset(&ctx->strm_);
      break;
    case INFLATE:
    case INFLATERAW:
    case GUNZIP:
      ctx->err_ = inflateReset(&ctx->strm_);
      break;
    default:
      break;
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Failed to reset stream");
    }
  }

  void ZCtx::Error(ZCtx* ctx, const char* message) {
    if (ctx->strm_.msg != nullptr) {
      message = ctx->strm_.msg;
    }

    if (ctx->errorCb_ && message) ctx->errorCb_(message, ctx->err_);

    ctx->write_in_progress_ = false;
    if (ctx->pending_close_)
      ctx->Close();
  }

  bool ZCtx::CheckError(ZCtx* ctx) {
    // Acceptable error states depend on the type of zlib stream.
    switch (ctx->err_) {
    case Z_OK:
    case Z_BUF_ERROR:
      if (ctx->strm_.avail_out != 0 && ctx->flush_ == Z_FINISH) {
        ZCtx::Error(ctx, "unexpected end of file");
        return false;
      }
    case Z_STREAM_END:
      // normal statuses, not fatal
      break;
    case Z_NEED_DICT:
      if (ctx->dictionary_ == nullptr)
        ZCtx::Error(ctx, "Missing dictionary");
      else
        ZCtx::Error(ctx, "Bad dictionary");
      return false;
    default:
      // something else.
      ZCtx::Error(ctx, "Zlib error");
      return false;
    }

    return true;
  }

  void ZCtx::AfterSync(ZCtx* ctx) {
//     Environment* env = ctx->env();
//     Local<Integer> avail_out = Integer::New(env->isolate(),
//       ctx->strm_.avail_out);
//     Local<Integer> avail_in = Integer::New(env->isolate(),
//       ctx->strm_.avail_in);

    ctx->write_in_progress_ = false;

//     Local<Array> result = Array::New(env->isolate(), 2);
//     result->Set(0, avail_in);
//     result->Set(1, avail_out);
//     args.GetReturnValue().Set(result);
  }

  void ZCtx::After(uv_work_t* work_req, int status) {
    CHECK_EQ(status, 0);

    ZCtx* ctx = ContainerOf(&ZCtx::work_req_, work_req);

    if (!CheckError(ctx))
      return;

    ctx->write_in_progress_ = false;

    // call the write() cb
    if (ctx->writeCb_) ctx->writeCb_(ctx->strm_.avail_in, ctx->strm_.avail_out);
    if (ctx->pending_close_)
      ctx->Close();
  }
}
