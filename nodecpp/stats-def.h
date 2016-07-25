#pragma once
#include "moment.h"

namespace nodecpp {
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

    bool isFile();
    bool isDirectory();
    bool isCharacterDevice();
    bool isSymbolicLink();
  };
}