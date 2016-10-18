#include "console.h"
#include "fs.h"
#include "path.h"

namespace nodecpp {

  void Console::outputConsole(level::LEVEL_ENUM l) {
    console_ = spd::stdout_logger_mt("console", false);
  }

  void Console::outputDbgview(level::LEVEL_ENUM l) {
    outputDbgViewLevel_ = l;
  }

  void Console::outputFile(level::LEVEL_ENUM l, const string& logPrefix, const string& logPath, level::LEVEL_ENUM flushLevel) {
    if (fs.existsSync(logPath)) {
      fileConsole_ = spd::daily_logger_mt(logPrefix, path.join(logPath, logPrefix));
      fileConsole_->set_level(spd::level::level_enum(l));

      // trigger flush if the log severity is info or higher
      fileConsole_->flush_on(spd::level::level_enum(flushLevel));
    }
  }

  void Console::outputAsyncFile(level::LEVEL_ENUM l, const string& logPrefix, const string& logFile) {

  }

  Console& console = Console::instance();

}
