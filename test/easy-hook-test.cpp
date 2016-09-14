#include "stdafx.h"
#include "easy-hook-test.h"
#include <nodecpp/easy-hook.h>

BOOL WINAPI myBeepHook(DWORD dwFreq, DWORD dwDuration)
{
  cout << "\n****All your beeps belong to us! Freq: " << dwFreq << "\n\n";
  return Beep(dwFreq + 800, dwDuration);
}

TEST_F(EasyHookTest, beep) {
  easyhook.addFuncInfo("kernel32", "Beep", myBeepHook);
  Beep(500, 500);
  easyhook.hook();
  Beep(500, 500);
  easyhook.unhook();
  Beep(500, 500);
/*
  HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
  cout << "Beep Address: " << GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep");

  // Install the hook
  NTSTATUS result = LhInstallHook(
    GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep"),
    myBeepHook,
    NULL,
    &hHook);
  if (FAILED(result))
  {
    wstring s(RtlGetLastErrorString());
    wcout << "Failed to install hook: ";
    wcout << s;
    return;
  }

  cout << "Beep after hook installed but not enabled.\n";
  Beep(500, 500);

  cout << "Activating hook for current thread only.\n";
  // If the threadId in the ACL is set to 0, 
  // then internally EasyHook uses GetCurrentThreadId()
  ULONG ACLEntries[1] = { 0 };
  LhSetInclusiveACL(ACLEntries, 1, &hHook);

  cout << "Beep after hook enabled.\n";
  Beep(500, 500);

  cout << "Uninstall hook\n";
  LhUninstallHook(&hHook);

  cout << "Beep after hook uninstalled\n";
  Beep(500, 500);

  cout << "\n\nRestore ALL entry points of pending removals issued by LhUninstallHook()\n";
  LhWaitForPendingRemovals();*/
}