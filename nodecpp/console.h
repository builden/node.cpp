#pragma once
#include "nodecpp-def.h"
#include "singleton.h"

#define SPDLOG_FMT_EXTERNAL
#include "spdlog/spdlog.h"
namespace spd = spdlog;

namespace nodecpp {
  namespace level {
    typedef enum {
      trace = 0,
      debug = 1,
      info = 2,
      warn = 3,
      err = 4,
      critical = 5,
      off = 6
    } LEVEL_ENUM;
  }

  class Console : public Singleton<Console> {
  public:
    // Console();

    void outputConsole(level::LEVEL_ENUM l);
    void outputDbgview(level::LEVEL_ENUM l);
    void outputFile(level::LEVEL_ENUM l, const string& logPrefix, const string& logPath);
    void outputAsyncFile(level::LEVEL_ENUM l, const string& logPrefix, const string& logPath);

    template <typename... Args> void trace(const char* fmt, const Args&... args) {
      if (console_) console_->trace(fmt, args...);
      if (fileConsole_) fileConsole_->trace(fmt, args...);
      if (outputDbgViewLevel_ <= level::trace) fmt::odebug(fmt, args...);
    }

    template <typename... Args> void debug(const char* fmt, const Args&... args) {
      if (console_) console_->debug(fmt, args...);
      if (fileConsole_) fileConsole_->debug(fmt, args...);
      if (outputDbgViewLevel_ <= level::debug) fmt::odebug(fmt, args...);
    }

    template <typename... Args> void log(const char* fmt, const Args&... args) {
      if (console_) console_->info(fmt, args...);
      if (fileConsole_) fileConsole_->info(fmt, args...);
      if (outputDbgViewLevel_ <= level::info) fmt::odebug(fmt, args...);
    }

    template <typename... Args> void info(const char* fmt, const Args&... args) {
      if (console_) console_->info(fmt, args...);
      if (fileConsole_) fileConsole_->info(fmt, args...);
      if (outputDbgViewLevel_ <= level::info) fmt::odebug(fmt, args...);
    }

    template <typename... Args> void warn(const char* fmt, const Args&... args) {
      if (console_) console_->warn(fmt, args...);
      if (fileConsole_) fileConsole_->warn(fmt, args...);
      if (outputDbgViewLevel_ <= level::warn) fmt::odebug(fmt, args...);
    }

    template <typename... Args> void error(const char* fmt, const Args&... args) {
      if (console_) console_->error(fmt, args...);
      if (fileConsole_) fileConsole_->error(fmt, args...);
      if (outputDbgViewLevel_ <= level::error) fmt::odebug(fmt, args...);
    }

    template <typename T> void trace(const T& msg) {
      if (console_) console_->trace(msg);
      if (fileConsole_) fileConsole_->trace(msg);
      if (outputDbgViewLevel_ <= level::trace) fmt::odebug("{}", msg);
    }

    template <typename T> void debug(const T& msg) {
      if (console_) console_->debug(msg);
      if (fileConsole_) fileConsole_->debug(msg);
      if (outputDbgViewLevel_ <= level::debug) fmt::odebug("{}", msg);
    }

    template <typename T> void info(const T& msg) {
      if (console_) console_->info(msg);
      if (fileConsole_) fileConsole_->info(msg);
      if (outputDbgViewLevel_ <= level::info) fmt::odebug("{}", msg);
    }

    template <typename T> void log(const T& msg) {
      if (console_) console_->info(msg);
      if (fileConsole_) fileConsole_->info(msg);
      if (outputDbgViewLevel_ <= level::info) fmt::odebug("{}", msg);
    }

    template <typename T> void warn(const T& msg) {
      if (console_) console_->warn(msg);
      if (fileConsole_) fileConsole_->warn(msg);
      if (outputDbgViewLevel_ <= level::warn) fmt::odebug("{}", msg);
    }

    template <typename T> void error(const T& msg) {
      if (console_) console_->error(msg);
      if (fileConsole_) fileConsole_->error(msg);
      if (outputDbgViewLevel_ <= level::error) fmt::odebug("{}", msg);
    }

  private:
//     class impl;
//     unique_ptr<impl> pimpl;
    shared_ptr<spd::logger> console_ = nullptr;
    shared_ptr<spd::logger> fileConsole_ = nullptr;
    level::LEVEL_ENUM outputDbgViewLevel_ = level::off;
  };

  extern Console& console;
}
