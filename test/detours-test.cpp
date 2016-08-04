#include "stdafx.h"
#include "detours-test.h"
#include <nodecpp/detours-hooker.h>

using Beep_t =  BOOL(WINAPI*)(DWORD dwFreq, DWORD dwDuration);
Beep_t g_oldBeep = nullptr;
BOOL WINAPI newBeep(DWORD dwFreq, DWORD dwDuration) {
  return g_oldBeep(dwFreq + 500, dwDuration);
}

TEST_F(DetoursTest, beep) {
  detours.addFuncInfo("kernel32", "Beep", (void **)&g_oldBeep, newBeep);
  Beep(500, 500);
  detours.hook();
  Beep(500, 500);
  detours.unhook();
  Beep(500, 500);
}