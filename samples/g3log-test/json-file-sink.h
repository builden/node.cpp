#pragma once
#include <g3log/logmessage.hpp>

namespace g3 {

  class JsonFileSink {
  public:
    JsonFileSink(const std::string &log_prefix, const std::string &log_directory, const std::string &logger_id = "g3log");
    virtual ~JsonFileSink();

    void fileWrite(LogMessageMover message);
    std::string changeLogFile(const std::string &directory, const std::string &logger_id);
    std::string fileName();


  private:
    std::string _log_file_with_path;
    std::string _log_prefix_backup; // needed in case of future log file changes of directory
    std::unique_ptr<std::ofstream> _outptr;

    void addLogFileHeader();
    std::ofstream &filestream() {
      return *(_outptr.get());
    }


    JsonFileSink &operator=(const JsonFileSink &) = delete;
    JsonFileSink(const JsonFileSink &other) = delete;

  };
}