#pragma once

#include "nodecpp-def.h"
#include "core-math.h"
#include "buffer.h"
#include "iconv.h"
#include "underscore.string.h"
#include "lodash.h"
#include "core.h"

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

namespace nodecpp {
  // nodecpp����Ϣѭ��������
  // ���е����鴦����ɺ��Զ��Ƴ�
  void run();

  // ��ʱ�����¼����У���������Ĭ��1����
  void runInterval(int interval = 1);

  // ����һ���¼�������
  void runOnce();
  // ���¼�����������
  void runNoWait();
}

#pragma comment(lib, "nodecpp.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libuv.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
