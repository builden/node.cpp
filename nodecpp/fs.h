/*
// flags
'r' - Open file for reading. An exception occurs if the file does not exist.
'r+' - Open file for reading and writing. An exception occurs if the file does not exist.
'rs' - Open file for reading in synchronous mode. Instructs the operating system to bypass the local file system cache.
This is primarily useful for opening files on NFS mounts as it allows you to skip the potentially stale local cache. It has a very real impact on I/O performance so don't use this flag unless you need it.
Note that this doesn't turn fs.open() into a synchronous blocking call. If that's what you want then you should be using fs.openSync()
'rs+' - Open file for reading and writing, telling the OS to open it synchronously. See notes for 'rs' about using this with caution.
'w' - Open file for writing. The file is created (if it does not exist) or truncated (if it exists).
'wx' - Like 'w' but fails if path exists.
'w+' - Open file for reading and writing. The file is created (if it does not exist) or truncated (if it exists).
'wx+' - Like 'w+' but fails if path exists.
'a' - Open file for appending. The file is created if it does not exist.
'ax' - Like 'a' but fails if path exists.
'a+' - Open file for reading and appending. The file is created if it does not exist.
'ax+' - Like 'a+' but fails if path exists.
*/
#ifndef __FS_H__
#define __FS_H__
#include "nodecpp-def.h"
#include "buffer.h"
#include <uv.h>
#include "error.h"
#include "singleton.h"
#include "moment.h"

#include <iostream>

namespace nodecpp {
  using std::map;

  using OpenCb_t = function<void(const Error&, int)>;
  using ReadCb_t = function<void(const Error&, const Buffer&)>;
  using ReadStrCb_t = function<void(const Error&, const string&)>;
  using WriteCb_t = function<void(const Error&)>;

  class FsWrap {
  public:
    FsWrap(int fd): fd_(fd) {}

    void readFile(ReadCb_t cb);
    void writeFile(const Buffer& buf, WriteCb_t cb);

  private:
    static void readCb(uv_fs_t* req);
    static void writeCb(uv_fs_t* req);
  private:
    uv_buf_t iov_;
    char buffer_[1024];
    int fd_;
    uv_fs_t req_;
    Buffer buf_;
  };

  class Fs : public Singleton<Fs> {
  public:
    class Stats {
    public:
      uint64_t dev = 0;
      uint64_t mode = 0;
      uint64_t nlink = 0;
      uint64_t uid = 0;
      uint64_t gid = 0;
      uint64_t rdev = 0;
      uint64_t blksize = 0;
      uint64_t ino = 0;
      uint64_t size = 0;
      uint64_t blocks = 0;

      //  "Access Time" - Time when file data last accessed. Changed by the mknod(2), utimes(2), and read(2) system calls.
      Moment atime;
      // "Modified Time" - Time when file data last modified. Changed by the mknod(2), utimes(2), and write(2) system calls.
      Moment mtime;
      // "Change Time" - Time when file status was last changed (inode data modification). Changed by the chmod(2), chown(2), link(2), mknod(2), rename(2), unlink(2), utimes(2), read(2), and write(2) system calls.
      Moment ctime;
      // "Birth Time" - Time of file creation. Set once when the file is created. On filesystems where birthtime is not available, this field may instead hold either the ctime or 1970-01-01T00:00Z (ie, unix epoch timestamp 0). Note that this value may be greater than atime or mtime in this case. On Darwin and other FreeBSD variants, also set if the atime is explicitly set to an earlier value than the current birthtime using the utimes(2) system call.
      Moment birthtime;

      bool isFile() { return ((mode & S_IFMT) == S_IFREG); }
      bool isDirectory() { return ((mode & S_IFMT) == S_IFDIR); }
      bool isCharacterDevice() { return ((mode & S_IFMT) == S_IFCHR); }
      bool isSymbolicLink() { return ((mode & S_IFMT) == S_IFLNK); }
//       bool isFIFO() { return mode & S_IFIFO; }
//       bool isSocket() { return mode & S_IFSOCK; }
    };
    using StatCb_t = function<void(const Error&, const Stats&)>;
    using ExistsCb_t = function<void(bool)>;
    using RmdirCb_t = function<void(const Error&)>;
    using UnlinkCb_t = function<void(const Error&)>;
    using MkdirCb_t = function<void(const Error&)>;
    using RenameCb_t = function<void(const Error&)>;

  public:
    void open(const string& path, const string& flags, OpenCb_t cb);
    void readFile(const string& path, ReadCb_t cb);
    void readFile(const string& path, const string& encoding, ReadStrCb_t cb);

    Buffer readFileSync(const string& path);
    string readFileSync(const string& path, const string& encoding);

    void writeFile(const string& path, const Buffer& data, WriteCb_t cb);
    void writeFileSync(const string& path, const Buffer& data);

    void stat(const string& path, StatCb_t cb);
    Stats statSync(const string& path);

    void exists(const string& path, ExistsCb_t cb);
    bool existsSync(const string& path);

    // 创建目录
    void mkdir(const string& path, MkdirCb_t cb);
    void mkdirSync(const string& path);

    // 删除目录，只能删除空文件夹
    void rmdir(const string& path, RmdirCb_t cb);
    void rmdirSync(const string& path);

    // 删除文件
    void unlink(const string& path, UnlinkCb_t cb);
    void unlinkSync(const string& path);

    // 重命名
    void rename(const string& oldPath, const string& newPath, RenameCb_t cb);
    void renameSync(const string& oldPath, const string& newPath);

  private:
    int stringToFlags(const string& path);
  };

  extern Fs &fs;

}

#endif // !__FS_H__
