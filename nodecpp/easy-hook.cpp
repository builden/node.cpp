#include "easy-hook.h"
#include <easyhook.h>
#include "fmt/format.h"

namespace nodecpp {
  EasyHook& easyhook = EasyHook::instance();

  class EasyHook::impl {
  public:
    bool addFuncInfo(const string& moduleName, const string& funcName, void *hookFunc) {
      if (hookFunc == nullptr) return false;
      HMODULE hMoudle = GetModuleHandleA(moduleName.c_str());
      if (hMoudle == NULL) {
        fmt::odebug("[nodecpp] GetModuleHandleA \"{}\" failed!", moduleName);

        hMoudle = LoadLibraryA(moduleName.c_str());
        if (hMoudle == NULL) {
          fmt::odebug("[nodecpp] LoadLibraryA \"{}\" failed!", moduleName);
          return false;
        }
      }

      // Install the hook
      NTSTATUS result = LhInstallHook(
        GetProcAddress(hMoudle, funcName.c_str()),
        hookFunc,
        NULL,
        &hHook_);
      if (FAILED(result)) {
        fmt::odebug(L"[nodecpp] LhInstallHook Failed to install hook: \"{}\" failed!", wstring(RtlGetLastErrorString()));
        return false;
      }

      return true;
    }

    void hook() {
      // If the threadId in the ACL is set to 0, 
      // then internally EasyHook uses GetCurrentThreadId()
      ULONG ACLEntries[1] = { 0 };
      LhSetInclusiveACL(ACLEntries, 1, &hHook_);
    }

    void unhook() {
      LhUninstallHook(&hHook_);
      LhWaitForPendingRemovals();
    }

  private:
    HOOK_TRACE_INFO hHook_ = { 0 };
  };

  EasyHook::EasyHook() : pimpl(new impl) {
    
  }

  bool EasyHook::addFuncInfo(const string& moduleName, const string& funcName, void *hookFunc) {
    return pimpl->addFuncInfo(moduleName, funcName, hookFunc);
  }

  void EasyHook::hook() {
    pimpl->hook();
  }

  void EasyHook::unhook() {
    pimpl->unhook();
  }

}