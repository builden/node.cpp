// PE file(exe, dll, ocx, cab, sys) Êý×ÖÇ©Ãû¼ì²â
#ifndef __PE_CERT_H__
#define __PE_CERT_H__

#include "nodecpp-def.h"
#include "singleton.h"

namespace nodecpp {
  class PeCert: public Singleton<PeCert> {
  public:
    bool verifyTrust(const wstring& filePath);
    void printCertInfo(const wstring& filePath);
  };

  extern PeCert& peCert;
}

#endif // !__PE_CERT_H__
