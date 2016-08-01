#include "child_process.h"
#include "underscore.string.h"
#include "process.h"
#include <windows.h>
#include <detours.h>
#pragma comment(lib, "detours.lib")

namespace nodecpp {

  bool OuterChildProcess::exec(const string& command) {
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };
    // DWORD dwFlags = CREATE_SUSPENDED;

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

  bool OuterChildProcess::execWithDll(const string& command, const string& dllPath) {
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

  shared_ptr<nodecpp::ChildProcess> OuterChildProcess::spawn(const string& command, json options) {
    svec_t args;
    return spawn(command, args, options);
  }


  shared_ptr<ChildProcess> OuterChildProcess::spawn(const string& command, const svec_t& args, json options /*= json()*/) {
    auto opts = normalizeSpawnArguments(command, args, options);
    shared_ptr<ChildProcess> child(new ChildProcess());
    child->spawn({
      { "file", opts["file"] },
      { "args", opts["args"] },
      { "cwd", options["cwd"] },
      { "windowsVerbatimArguments", options["windowsVerbatimArguments"] },
      { "detached", options["detached"] },
      { "envPairs", opts["envPairs"] },
      { "stdio", options["stdio"] },
      { "uid", options["uid"] },
      { "gid", options["gid"] }
    });
    return child;
 }


  json OuterChildProcess::normalizeSpawnArguments(const string& file, const svec_t& args, json options) {
    string innerFile = file;
    svec_t innerArgs = args;
    if (options["shell"].is_boolean() || options["shell"].is_string()) {
      string command = file + " " + s.join(args, " ");
      if (options["shell"].is_string()) {
        innerFile = options["shell"].get<string>();
      }
      else {
        innerFile = process.env["comspec"].is_string() ? process.env["comspec"].get<string>() : "cmd.exe";
      }
      innerArgs = { "/s", "/c", "\"" + command + "\"" };
      options["windowsVerbatimArguments"] = true;
    }

    innerArgs.insert(innerArgs.begin(), innerFile);

    json env = options["env"];
    if (!env.is_object()) env = process.env;
    svec_t envPairs;
    for (auto it = env.begin(); it != env.end(); ++it) {
      envPairs.emplace_back(it.key() + "=" + it.value().get<string>());
    }

    return{
      { "file", innerFile },
      { "args", innerArgs },
      { "options", options },
      { "envPairs", envPairs }
    };
  }

  OuterChildProcess &child_process = OuterChildProcess::instance();

}