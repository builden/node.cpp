#include "g3log.h"
#include "g3log-internal/json-file-sink.h"
#include "path.h"
#include "fs.h"
#include "process.h"

#include <g3log/logworker.hpp>
#include <g3log/logmessage.hpp>

namespace nodecpp {

  G3log& g3log = G3log::instance();

  class G3log::impl {
  public:
    void init(const string& logPrefix, const string& logPath, LOG_TYPE logType) {
      if (fs.existsSync(logPath)) {
        worker_ = g3::LogWorker::createLogWorker();
        if (logType == JSON) {
          worker_->addSink(std2::make_unique<g3::JsonFileSink>(logPrefix, logPath),
            &g3::JsonFileSink::fileWrite);
        }
        else {
          worker_->addDefaultLogger(logPrefix, logPath);
        }
        g3::initializeLogging(worker_.get());
      }
    }

    void finalize() {
      if (worker_) {
        worker_.release();
        worker_ = nullptr;
      }
    }

  private:
    std::unique_ptr<g3::LogWorker> worker_ = nullptr;
  };

  G3log::G3log() : pimpl(new impl) {}

  void G3log::init(const string& logPrefix, const string& logPath, LOG_TYPE logType /*= DEFAULT*/) {
    pimpl->init(logPrefix, logPath, logType);
  }

  void G3log::finalize() {
    pimpl->finalize();
  }

  void G3log::setLogLevel(int level) {

  }

}