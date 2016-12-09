#include "crypto.h"
#include <openssl/evp.h>

#ifdef _DEBUG
#ifdef _DLL
#pragma comment(lib, "openssl-d.lib")
#else  // _DLL
#pragma comment(lib, "openssl-sd.lib")
#endif // _DLL
#else  // _DEBUG
#ifdef _DLL
#pragma comment(lib, "openssl.lib")
#else  // _DLL
#pragma comment(lib, "openssl-s.lib")
#endif // _DLL
#endif // _DEBUG

namespace nodecpp {

  class Crypto::impl {
  public:
    bool createHash(const string& algorithm) {
      md_ = EVP_get_digestbyname(algorithm.c_str());
      if (md_ == nullptr) return false;
      EVP_MD_CTX_init(&mdctx_);
      if (EVP_DigestInit_ex(&mdctx_, md_, nullptr) <= 0) {
        return false;
      }

      initialised_ = true;
      finalized_ = false;
      return true;
    }

    bool update(const Buffer& buf) {
      if (!initialised_) return false;
      EVP_DigestUpdate(&mdctx_, (buf.size() == 0) ? nullptr : buf.data(), buf.size());
      return true;
    }

    Buffer digest() {
      uint8_t md_value[EVP_MAX_MD_SIZE] = { 0 };
      uint32_t md_len = 0;

      EVP_DigestFinal_ex(&mdctx_, md_value, &md_len);
      EVP_MD_CTX_cleanup(&mdctx_);
      finalized_ = true;
      return Buffer((const char*)md_value, md_len);
    }

    Buffer md5(const Buffer& buf) {
      if (createHash("md5") && update(buf)) return digest();
      return Buffer();
    }

  private:
    const EVP_MD* md_ = nullptr;
    EVP_MD_CTX mdctx_;
    bool initialised_ = false;
    bool finalized_ = true;
  };

  Crypto::Crypto() : pimpl(new impl) {
    OpenSSL_add_all_algorithms();
  }

  Crypto::~Crypto() {}

  Crypto& Crypto::createHash(const string& algorithm) {
    pimpl->createHash(algorithm);
    return *this;
  }

  Crypto& Crypto::update(const Buffer& buf) {
    pimpl->update(buf);
    return *this;
  }

  Buffer Crypto::digest() {
    return pimpl->digest();
  }

  nodecpp::Buffer Crypto::md5(const Buffer& buf) {
    return pimpl->md5(buf);
  }

  svec_t Crypto::getHashes() {
    return svec_t{ "DSA",
      "DSA-SHA",
      "DSA-SHA1",
      "DSA-SHA1-old",
      "RSA-MD4",
      "RSA-MD5",
      "RSA-MDC2",
      "RSA-RIPEMD160",
      "RSA-SHA",
      "RSA-SHA1",
      "RSA-SHA1-2",
      "RSA-SHA224",
      "RSA-SHA256",
      "RSA-SHA384",
      "RSA-SHA512",
      "dsaEncryption",
      "dsaWithSHA",
      "dsaWithSHA1",
      "dss1",
      "ecdsa-with-SHA1",
      "md4",
      "md4WithRSAEncryption",
      "md5",
      "md5WithRSAEncryption",
      "mdc2",
      "mdc2WithRSA",
      "ripemd",
      "ripemd160",
      "ripemd160WithRSA",
      "rmd160",
      "sha",
      "sha1",
      "sha1WithRSAEncryption",
      "sha224",
      "sha224WithRSAEncryption",
      "sha256",
      "sha256WithRSAEncryption",
      "sha384",
      "sha384WithRSAEncryption",
      "sha512",
      "sha512WithRSAEncryption",
      "shaWithRSAEncryption",
      "ssl2-md5",
      "ssl3-md5",
      "ssl3-sha1",
      "whirlpool" };
  }

  Crypto& crypto = Crypto::instance();
}