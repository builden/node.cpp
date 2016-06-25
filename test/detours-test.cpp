#include "stdafx.h"
#include "detours-test.h"
#include <nodecpp/hooker.h>

typedef BOOL(WINAPI* PFNBeep)(DWORD dwFreq, DWORD dwDuration);
void * g_oldBeep = NULL;
BOOL WINAPI newBeep(DWORD dwFreq, DWORD dwDuration) {
  return PFNBeep(g_oldBeep)(dwFreq + 500, dwDuration);
}

TEST_F(DetoursTest, beep) {
  hooker.addFuncInfo("kernel32", "Beep", &g_oldBeep, newBeep);
  Beep(500, 500);
  hooker.hook();
  Beep(500, 500);
  hooker.unhook();
  Beep(500, 500);
}