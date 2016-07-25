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
#include "fs-wrap.h"
#include "singleton.h"
#include "moment.h"
#include "stats-def.h"

#include <iostream>

namespace nodecpp {
  using OpenCb_t = function<void(const Error&, int)>;
  using CloseCb_t = function<void(const Error&)>;
  using ReadStrCb_t = function<void(const Error&, const string&)>;
  using StatCb_t = function<void(const Error&, const Stats&)>;
  using ExistsCb_t = function<void(bool)>;
  using AccessCb_t = function<void(const Error&)>;
  using RmdirCb_t = function<void(const Error&)>;
  using UnlinkCb_t = function<void(const Error&)>;
  using MkdirCb_t = function<void(const Error&)>;
  using RenameCb_t = function<void(const Error&)>;

  class Fs : public Singleton<Fs> {
  public:

  public:
    void open(const string& path, const string& flags, OpenCb_t cb);
    void open(const string& path, const string& flags, int mode, OpenCb_t cb);
    int openSync(const string& path, const string& flags, int mode = 0);

    void close(int fd, CloseCb_t cb);
    void closeSync(int fd);

    void readFile(const string& path, ReadCb_t cb);
    void readFile(const string& path, const string& encoding, ReadStrCb_t cb);

    Buffer readFileSync(const string& path);
    string readFileSync(const string& path, const string& encoding);

    void writeFile(const string& path, const Buffer& data, WriteCb_t cb);
    void writeFileSync(const string& path, const Buffer& data);

    void stat(const string& path, StatCb_t cb);
    Stats statSync(const string& path);
    void lstat(const string& path, StatCb_t cb);
    Stats lstatSync(const string& path);
    void fstat(int fd, StatCb_t cb);
    Stats fstat(int fd);

    void exists(const string& path, ExistsCb_t cb);
    bool existsSync(const string& path);

    // 创建目录
    void mkdir(const string& path, int mode, MkdirCb_t cb);
    void mkdir(const string& path, MkdirCb_t cb);
    void mkdirSync(const string& path, int mode = 0777);

    // 删除目录，只能删除空文件夹
    void rmdir(const string& path, RmdirCb_t cb);
    void rmdirSync(const string& path);

    // 删除文件
    void unlink(const string& path, UnlinkCb_t cb);
    void unlinkSync(const string& path);

    // 重命名
    void rename(const string& oldPath, const string& newPath, RenameCb_t cb);
    void renameSync(const string& oldPath, const string& newPath);

    void access(const string& path, int mode, AccessCb_t cb);
    void access(const string& path, AccessCb_t cb);
    void accessSync(const string& path, int mode = 0);

  private:
    int stringToFlags(const string& path);
  };

  extern Fs &fs;

}

#endif // !__FS_H__
