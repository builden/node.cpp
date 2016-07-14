#include "stdafx.h"
#include "json-file-sink.h"
#include "jsonfilesinkhelper.h"
#include <cassert>

namespace g3 {
  using namespace jsoninternal;

  JsonFileSink::JsonFileSink(const std::string &log_prefix, const std::string &log_directory, const std::string& logger_id)
    : _log_file_with_path(log_directory)
    , _log_prefix_backup(log_prefix)
    , _outptr(new std::ofstream)
  {
    _log_prefix_backup = prefixSanityFix(log_prefix);
    if (!isValidFilename(_log_prefix_backup)) {
      std::cerr << "g3log: forced abort due to illegal log prefix [" << log_prefix << "]" << std::endl;
      abort();
    }

    std::string file_name = createLogFileName(_log_prefix_backup, logger_id);
    _log_file_with_path = pathSanityFix(_log_file_with_path, file_name);
    _outptr = createLogFile(_log_file_with_path);

    if (!_outptr) {
      std::cerr << "Cannot write log file to location, attempting current directory" << std::endl;
      _log_file_with_path = "./" + file_name;
      _outptr = createLogFile(_log_file_with_path);
    }
    assert(_outptr && "cannot open log file at startup");
    addLogFileHeader();
  }


  JsonFileSink::~JsonFileSink() {
    Json exit_msg = Json::object{
      { "time", localtime_formatted(systemtime_now(), internal::time_formatted) },
      { "msg", "g3log g3FileSink shutdown" }
    };
    filestream() << exit_msg.dump() << std::flush;
    std::cerr << exit_msg.dump() << std::flush;
  }

  // The actual log receiving function
  void JsonFileSink::fileWrite(LogMessageMover message) {
    std::ofstream &out(filestream());
    auto msg = message.get();
    Json j = Json::object{
      { "time", msg.timestamp() },
      { "tick", msg.microseconds() },
      { "tid", msg.threadID() },
      { "msg", msg.message() },
      { "level", msg.level() },
      { "file", msg.file() },
      { "line", msg.line() },
      { "func", msg.function() }
    };
    out << j.dump() << "\n";
  }

  std::string JsonFileSink::changeLogFile(const std::string &directory, const std::string &logger_id) {

    auto now = g3::systemtime_now();
    auto now_formatted = g3::localtime_formatted(now, { internal::date_formatted + " " + internal::time_formatted });

    std::string file_name = createLogFileName(_log_prefix_backup, logger_id);
    std::string prospect_log = directory + file_name;
    std::unique_ptr<std::ofstream> log_stream = createLogFile(prospect_log);
    if (nullptr == log_stream) {
      filestream() << "\n" << now_formatted << " Unable to change log file. Illegal filename or busy? Unsuccessful log name was: " << prospect_log;
      return{}; // no success
    }

    addLogFileHeader();
    std::ostringstream ss_change;
    ss_change << "\n\tChanging log file from : " << _log_file_with_path;
    ss_change << "\n\tto new location: " << prospect_log << "\n";
    filestream() << now_formatted << ss_change.str();
    ss_change.str("");

    std::string old_log = _log_file_with_path;
    _log_file_with_path = prospect_log;
    _outptr = std::move(log_stream);
    ss_change << "\n\tNew log file. The previous log file was at: ";
    ss_change << old_log;
    filestream() << now_formatted << ss_change.str();
    return _log_file_with_path;
  }
  std::string JsonFileSink::fileName() {
    return _log_file_with_path;
  }
  void JsonFileSink::addLogFileHeader() {
    // filestream() << header();
  }

} // g3