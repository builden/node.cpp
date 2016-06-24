#include "shm.h"
#include "fmt/format.h"

namespace nodecpp {

  Shm::~Shm() {
    close();
  }

  bool Shm::create(const string& name, uint32_t size) {
    SECURITY_ATTRIBUTES	sa;
    SECURITY_DESCRIPTOR	sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, TRUE);
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = &sd;

    fileMapping_ = CreateFileMappingA(
      INVALID_HANDLE_VALUE,
      &sa,
      PAGE_READWRITE,
      0,
      size,
      name.c_str()
      );
    if (fileMapping_ == NULL) {
      fmt::odebug("[nodepp] CreateFileMapping failed {}", GetLastError());
      return false;
    }

    buf_ = (char *)MapViewOfFile(fileMapping_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (buf_ == nullptr) {
      fmt::odebug("[nodepp] MapViewOfFile failed {}", GetLastError());
      CloseHandle(fileMapping_);
      fileMapping_ = NULL;
      return false;
    }

    return true;
  }

  bool Shm::open(const string& name) {
    fileMapping_ = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
    if (fileMapping_ == NULL) {
      fmt::odebug("[nodepp] OpenFileMapping failed {}", GetLastError());
      return false;
    }

    buf_ = (char*)MapViewOfFile(fileMapping_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (buf_ == nullptr) {
      fmt::odebug("[nodepp] MapViewOfFile failed {}", GetLastError());
      CloseHandle(fileMapping_);
      fileMapping_ = NULL;
      return false;
    }

    return true;
  }

  const char* Shm::read() {
    return buf_;
  }

  bool Shm::write(const char* buf, uint32_t size) {
    if (buf_ != nullptr) {
      memcpy_s(buf_, size, buf, size);
      return true;
    }
    return false;
  }

  void Shm::close() {
    if (buf_ != nullptr) {
      UnmapViewOfFile(buf_);
      buf_ = nullptr;
    }

    if (fileMapping_ != NULL) {
      CloseHandle(fileMapping_);
      fileMapping_ = NULL;
    }
  }

}