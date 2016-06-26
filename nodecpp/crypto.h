#pragma once
#include "nodecpp-def.h"
#include "singleton.h"
#include "buffer.h"

namespace nodecpp {
  class Crypto : public Singleton<Crypto> {
  public:
    Crypto();
    ~Crypto();

    // algorithm: md5, md2, sha1, sha256, sha512 ... all from getHashes
    Crypto& createHash(const string& algorithm);
    Crypto& update(const Buffer& buf);
    Buffer digest();

    svec_t getHashes();

    // utility
    Buffer md5(const Buffer& buf);

  private:
    class impl;
    unique_ptr<impl> pimpl;
  };

  extern Crypto& crypto;
}
