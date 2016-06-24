#include "hooker.h"
#include "fmt/format.h"

#include <windows.h>
#include <detours/detours.h>

namespace nodepp {

  bool Hooker::addFuncInfo(const string& moduleName, const string& funcName, void **originFuncPtr, void *newFunc) {
    if (originFuncPtr == nullptr || newFunc == nullptr) return false;
    void * originFunc = nullptr;
    HMODULE hMoudle = GetModuleHandleA(moduleName.c_str());
    if (hMoudle == NULL) {
      fmt::odebug("[nodepp] GetModuleHandleA \"{}\" failed!", moduleName);

      hMoudle = LoadLibraryA(moduleName.c_str());
      if (hMoudle == NULL) {
        fmt::odebug("[nodepp] LoadLibraryA \"{}\" failed!", moduleName);
        return false;
      }
    }

    originFunc = GetProcAddress(hMoudle, funcName.c_str());
    if (originFunc != nullptr) {
      *originFuncPtr = originFunc;
      vecFuncInfo_.push_back(FuncInfo{ moduleName, funcName, originFuncPtr, newFunc });
      return true;
    }
    else {
      fmt::odebug("[nodepp] GetProcAddress {}:{} failed!", moduleName, funcName);
    }

    return false;
  }

  void Hooker::hook() {
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& funcInfo : vecFuncInfo_) {
      DetourAttach(&(*funcInfo.originFuncPtr), funcInfo.newFunc);
    }
    LONG commitRst = DetourTransactionCommit();
  }

  void Hooker::unhook() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& funcInfo : vecFuncInfo_) {
      DetourDetach(&(*funcInfo.originFuncPtr), funcInfo.newFunc);
    }
    DetourTransactionCommit();
  }

  Hooker &hooker = Hooker::instance();
}