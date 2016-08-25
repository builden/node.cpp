#include "node-zlib.h"
#include "path.h"
#include "fs.h"
#include "fmt/format.h"

#include "zlib/unzip.h"
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

  class UnzipFileWrap {
  public:
    UnzipFileWrap() {
      req_.data = this;
    }

    void unzipFile(const string& zipFile, const string& destPath, UnzipFileCb_t cb) {
      zipFile_ = zipFile;
      destPath_ = destPath;
      cb_ = cb;
      uv_queue_work(uv_default_loop(), &req_, runUnzipFile, afterUnzipFile);
    }

    static void runUnzipFile(uv_work_t* req) {
      auto wrap = (UnzipFileWrap*)req->data;
      try {
        zlib.unzipFileSync(wrap->zipFile_, wrap->destPath_);
      }
      catch (const Error& err) {
        wrap->errMsg_ = err.str();
      }
    }

    static void afterUnzipFile(uv_work_t* req, int status) {
      auto wrap = (UnzipFileWrap*)req->data;
      if (wrap->errMsg_.empty()) {
        wrap->cb_(Error());
      }
      else {
        wrap->cb_(Error(wrap->errMsg_));
      }
      delete wrap;
    }

  private:
    uv_work_t req_;
    string zipFile_;
    string destPath_;
    UnzipFileCb_t cb_;
    string errMsg_;
  };

  void Zlib::unzipFile(const string& zipFile, const string& destPath, UnzipFileCb_t cb) {
    auto wrap = new UnzipFileWrap();
    wrap->unzipFile(zipFile, destPath, cb);
  }

  void Zlib::unzipFileSync(const string& zipFile, const string& destPath) {
    unz_file_info64 FileInfo;
    unzFile zFile = unzOpen64(zipFile.c_str());
    if (nullptr == zFile) {
      throw Error(fmt::format("unzOpen64 {} failed", zipFile));
      return;
    }
    fs.mkdirsSync(destPath);

    unz_global_info64 gi;
    if (unzGetGlobalInfo64(zFile, &gi) != UNZ_OK) {
      throw Error(fmt::format("unzGetGlobalInfo64 {} failed", zipFile));
      return;
    }

    int result;
    for (int i = 0; i < gi.number_entry; ++i) {
      char file[256] = { 0 };
      if (unzGetCurrentFileInfo64(zFile, &FileInfo, file, sizeof(file), nullptr, 0, nullptr, 0) != UNZ_OK) {
        throw Error(fmt::format("unzGetCurrentFileInfo64 {} failed", zipFile));
      }

      // 文件，否则为目录
      if (!(FileInfo.external_fa & FILE_ATTRIBUTE_DIRECTORY)) {
        // 打开文件
        result = unzOpenCurrentFile(zFile);
        // result = unzOpenCurrentFilePassword(zFile, "szPassword");

        string destFilePath = path.join(destPath, file);
        int fd = fs.openSync(destFilePath, "w", 0666);
        Buffer data(1024);
        int size;
        //读取内容
        while (true) {
          size = unzReadCurrentFile(zFile, const_cast<char*>(data.data()), 1024);
          if (size <= 0) break;
          fs.writeSync(fd, data, 0, size);
        }
      }
      else {
        string destDirPath = path.join(destPath, file);
        fs.mkdirsSync(destDirPath);
      }

      //关闭当前文件
      unzCloseCurrentFile(zFile);

      //出错
      if (i < gi.number_entry - 1 && unzGoToNextFile(zFile) != UNZ_OK) {
        throw Error(fmt::format("unzGoToNextFile {} failed", zipFile));
      }
    }
    unzClose(zFile);
  }

}