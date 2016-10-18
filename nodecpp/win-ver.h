#pragma once
#include <windows.h>

#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8                   0x0602
#define _WIN32_WINNT_WINBLUE                0x0603
#endif

namespace nodecpp {

  inline bool isWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor) {
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(
      VerSetConditionMask(
        VerSetConditionMask(
          0, VER_MAJORVERSION, VER_GREATER_EQUAL),
        VER_MINORVERSION, VER_GREATER_EQUAL),
      VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = wMajorVersion;
    osvi.dwMinorVersion = wMinorVersion;
    osvi.wServicePackMajor = wServicePackMajor;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
  }

  // xp
  inline bool isWindowXPOrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
  }

  // xp sp1
  inline bool isWindowsXPSP1OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
  }

  // xp sp2
  inline bool isWindowsXPSP2OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
  }

  // xp sp3
  inline bool isWindowsXPSP3OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
  }

  // vista
  inline bool isWindowsVistaOrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
  }

  // vista sp1
  inline bool isWindowsVistaSP1OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
  }

  // vista sp2
  inline bool isWindowsVistaSP2OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
  }

  // win7
  inline bool isWindows7OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
  }

  // win7 sp1
  inline bool isWindows7SP1OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
  }

  // PlatformToolset v140_xp not support
  // win8
  inline bool isWindows8OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
  }

  // win8 sp1
  inline bool isWindows8Point1OrGreater() {
    return isWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
  }

  // windows server
  inline bool isWindowsServer() {
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0, 0, VER_NT_WORKSTATION };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);

    return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
  }

} // namespace nodecpp