#include "console.h"
#include "fs.h"
#include "path.h"

namespace nodecpp {
//   class Console::impl {
//   public:
//     shared_ptr<spd::logger> console_ = nullptr;
//     shared_ptr<spd::logger> fileConsole_ = nullptr;
//   };
// 
//   Console::Console() : pimpl(new impl()) {}

  void Console::outputConsole(level::LEVEL_ENUM l) {
    // pimpl->console_ = spd::stdout_logger_mt("console", false);
    console_ = spd::stdout_logger_mt("console", false);
  }

  void Console::outputDbgview(level::LEVEL_ENUM l) {
    outputDbgViewLevel_ = l;
  }

  void Console::outputFile(level::LEVEL_ENUM l, const string& logPrefix, const string& logPath) {
    if (fs.existsSync(logPath)) {
      fileConsole_ = spd::daily_logger_mt(logPrefix, path.join(logPath, logPrefix));
      fileConsole_->set_level(spd::level::level_enum(l));

      // trigger flush if the log severity is error or higher
      fileConsole_->flush_on(spd::level::level_enum(level::warn));
    }
  }

  void Console::outputAsyncFile(level::LEVEL_ENUM l, const string& logPrefix, const string& logFile) {

  }

  Console& console = Console::instance();

}
