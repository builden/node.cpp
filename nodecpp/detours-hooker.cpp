#include "detours-hooker.h"
#include "fmt/format.h"

#include <windows.h>
#include <detours.h>
#pragma comment(lib, "detours.lib")

namespace nodecpp {

  bool Detours::addFuncInfo(const string& moduleName, const string& funcName, void **originFuncPtr, void *newFunc) {
    if (originFuncPtr == nullptr || newFunc == nullptr) return false;
    void * originFunc = nullptr;
    HMODULE hMoudle = GetModuleHandleA(moduleName.c_str());
    if (hMoudle == NULL) {
      fmt::odebug("[nodecpp] GetModuleHandleA \"{}\" failed!", moduleName);

      hMoudle = LoadLibraryA(moduleName.c_str());
      if (hMoudle == NULL) {
        fmt::odebug("[nodecpp] LoadLibraryA \"{}\" failed!", moduleName);
        return false;
      }
    }

    originFunc = GetProcAddress(hMoudle, funcName.c_str());
    if (originFunc != nullptr) {
      *originFuncPtr = originFunc;
      vecFuncInfo_.push_back(FuncInfo{ moduleName, funcName, originFuncPtr, newFunc });
      fmt::odebug("[nodecpp] GetProcAddress {}:{} succ!", moduleName, funcName);
      return true;
    }
    else {
      fmt::odebug("[nodecpp] GetProcAddress {}:{} failed!", moduleName, funcName);
    }

    return false;
  }

  void Detours::hook() {
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& funcInfo : vecFuncInfo_) {
      DetourAttach(&(*funcInfo.originFuncPtr), funcInfo.newFunc);
    }
    LONG commitRst = DetourTransactionCommit();
    if (commitRst == NO_ERROR) {
      fmt::odebug("[nodecpp] hook succ");
    }
    else {
      fmt::odebug("[nodecpp] hook failed {}", commitRst);
    }
  }

  void Detours::unhook() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& funcInfo : vecFuncInfo_) {
      DetourDetach(&(*funcInfo.originFuncPtr), funcInfo.newFunc);
    }
    DetourTransactionCommit();
  }

  bool Detours::execWithDll(const string& command, const string& dllPath) {
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    // DWORD dwFlags = CREATE_SUSPENDED;
    BOOL rst = DetourCreateProcessWithDllA(
      NULL,  // Application Name
      const_cast<LPSTR>(command.c_str()),  // CommandLine
      NULL,  // process attribute
      NULL,
      FALSE,
      0,
      NULL,
      NULL,
      &si,
      &pi,
      dllPath.c_str(),
      NULL
    );

    if (!rst) {
      // std::cout << "Get Error: " << GetLastError();
      return false;
    }

    ResumeThread(pi.hThread);
    return true;
  }

  Detours &detours = Detours::instance();
}