#pragma once

#include "nodecpp-def.h"
#include "core-math.h"
#include "buffer.h"
#include "iconv.h"
#include "underscore.string.h"
#include "lodash.h"
#include "core.h"
#include "stream-readable.h"
#include "console.h"

#include "crypto.h"
#include "timers.h"
#include "process.h"
#include "path.h"
#include "os.h"
#include "events.h"
#include "child_process.h"
#include "querystring.h"
#include "url.h"
#include "http.h"
#include "node-zlib.h"
#include "dns.h"

#include "fs.h"
#include "net.h"
#include "async.h"

namespace nodecpp {
  // nodecpp的消息循环，阻塞
  // 所有的事情处理完成后，自动推出
  void run();

  // 定时遍历事件序列，非阻塞，默认1毫秒
  void runInterval(int interval = 1);

  // 处理一个事件，阻塞
  void runOnce();
  // 有事件则处理，非阻塞
  void runNoWait();
}

#ifdef _DEBUG

#ifdef _DLL
#pragma comment(lib, "nodecpp-d.lib")
#pragma comment(lib, "libuv-d.lib")
#else  // _DLL
#pragma comment(lib, "nodecpp-sd.lib")
#pragma comment(lib, "libuv-sd.lib")
#endif // _DLL

#else  // _DEBUG

#ifdef _DLL
#pragma comment(lib, "nodecpp.lib")
#pragma comment(lib, "libuv.lib")
#else  // _DLL
#pragma comment(lib, "nodecpp-s.lib")
#pragma comment(lib, "libuv-s.lib")
#endif // _DLL

#endif // _DEBUG

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")