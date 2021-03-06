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
#include "singleton.h"
#include "moment.h"
#include "stats-def.h"
#include "read-file-context.h"

namespace nodecpp {
  using OpenCb_t = function<void(const Error&, int)>;
  using CloseCb_t = function<void(const Error&)>;
  using ReadStrCb_t = function<void(const Error&, const string&)>;
  using StatCb_t = function<void(const Error&, const Stats&)>;
  using ExistsCb_t = function<void(bool)>;
  using AccessCb_t = function<void(const Error&)>;
  using RmdirCb_t = function<void(const Error&)>;
  using UnlinkCb_t = function<void(const Error&)>;
  using RemoveCb_t = function<void(const Error&)>;
  using MkdirCb_t = function<void(const Error&)>;
  using RenameCb_t = function<void(const Error&)>;
  using WriteAsyncCb_t = function<void(const Error&, uint32_t)>;
  using ReadCb_t = function<void(const Error&, const Buffer&)>;
  using WriteCb_t = function<void(const Error&)>;
  using ReadDirCb_t = function<void(const Error&, const svec_t&)>;

  class Fs : public Singleton<Fs> {
  public:

  public:
    void open(const string& p, const string& flags, OpenCb_t cb);
    void open(const string& p, const string& flags, int mode, OpenCb_t cb);
    int openSync(const string& p, const string& flags, int mode = 0);

    void close(int fd, CloseCb_t cb);
    void closeSync(int fd);

    void readFile(const string& p, ReadCb_t cb);
    void readFile(const string& p, const string& encoding, ReadStrCb_t cb);

    Buffer readFileSync(const string& p);
    string readFileSync(const string& p, const string& encoding);

    void writeFile(const string& p, const Buffer& data, WriteCb_t cb);
    void writeFileSync(const string& p, const Buffer& data);

    void stat(const string& p, StatCb_t cb);
    Stats statSync(const string& p);
    // 当路径是文件链接时，返回这个链接文件的信息
    void lstat(const string& p, StatCb_t cb);
    Stats lstatSync(const string& p);
    void fstat(int fd, StatCb_t cb);
    Stats fstatSync(int fd);

    void exists(const string& p, ExistsCb_t cb);
    bool existsSync(const string& p);

    // 创建目录
    void mkdir(const string& p, int mode, MkdirCb_t cb);
    void mkdir(const string& p, MkdirCb_t cb);
    void mkdirSync(const string& p, int mode = 0777);
    void mkdirs(const string& p, MkdirCb_t cb);
    void mkdirsSync(const string& p);

    void readdir(const string& p, ReadDirCb_t cb);
    svec_t readdirSync(const string& p);

    // 删除目录，只能删除空文件夹
    void rmdir(const string& p, RmdirCb_t cb);
    void rmdirSync(const string& p);

    // 删除文件
    void unlink(const string& p, UnlinkCb_t cb);
    void unlinkSync(const string& p);

    // 删除文件或文件夹
    void remove(const string& p, RemoveCb_t cb);
    void removeSync(const string& p);

    // 重命名
    void rename(const string& oldPath, const string& newPath, RenameCb_t cb);
    void renameSync(const string& oldPath, const string& newPath);

    void access(const string& p, int mode, AccessCb_t cb);
    void access(const string& p, AccessCb_t cb);
    void accessSync(const string& p, int mode = 0);

    uint32_t readSync(int fd, Buffer& buffer, uint32_t offset, uint32_t length, int64_t position = -1);

    void write(int fd, const Buffer& buffer, uint32_t offset, uint32_t length, int64_t position, WriteAsyncCb_t cb);
    uint32_t writeSync(int fd, const Buffer& buffer, uint32_t offset, uint32_t length, int64_t position = -1);

    json readJsonSync(const string& file);

    // just files, have not dir
    svec_t walkSync(const string& p);

    // link    创建hard link, 创建备份
    // symlink 创建符号链接（软链接）快捷方式
    // 创建文件夹的快捷方式 fs.symlink(srcPath, destPath, "dir")
  private:
    int stringToFlags(const string& p);
    uint32_t tryReadSync(int fd, Buffer& buffer, uint32_t pos, uint32_t len);
    void writeAll(int fd, bool isUserFd, const Buffer& buffer, uint32_t offset, uint32_t length, int64_t position, WriteCb_t cb);
    svec_t recursiveWalkSync(const string& p, svec_t& filelist);
  };

  extern Fs &fs;

}

#endif // !__FS_H__
