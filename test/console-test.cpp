#include "stdafx.h"
#include "console-test.h"
#include <nodecpp/console.h>

TEST_F(ConsoleTest, toConsole) {
  console.outputConsole(level::trace);
  auto logPath = path.join(path.dirname(process.execPath), "logs");
  console.outputFile(level::info, "daylogs", logPath);
  console.outputDbgview(level::trace);

  console.log("[nodecpp] abc");
  console.log("[nodecpp] format string {1} {0}", "cbd", "abc"); // format string abc cbd
}