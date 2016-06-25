#include "child_process.h"
#include <windows.h>
#include <detours.h>
#pragma comment(lib, "detours.lib")

namespace nodecpp {

  bool ChildProcess::exec(const string& command) {
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    DWORD dwFlags = CREATE_SUSPENDED;

    BOOL rst = CreateProcessA(
      NULL,
      const_cast<LPSTR>(command.c_str()),
      NULL,
      NULL,
      FALSE,
      0,
      NULL,
      NULL,
      &si,
      &pi
      );

    if (!rst) {
      return false;
    }
    ResumeThread(pi.hThread);
    return true;
  }

  bool ChildProcess::execWithDll(const string& command, const string& dllPath) {
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    DWORD dwFlags = CREATE_SUSPENDED;
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

  ChildProcess &child_process = ChildProcess::instance();

}