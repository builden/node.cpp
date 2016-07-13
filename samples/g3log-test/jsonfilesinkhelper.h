#pragma once


#include <memory>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace g3 {
  namespace jsoninternal {
    bool isValidFilename(const std::string &prefix_filename);
    std::string prefixSanityFix(std::string prefix);
    std::string pathSanityFix(std::string path, std::string file_name);
    std::string header();
    std::string createLogFileName(const std::string &verified_prefix, const std::string &logger_id);
    bool openLogFile(const std::string &complete_file_with_path, std::ofstream &outstream);
    std::unique_ptr<std::ofstream> createLogFile(const std::string &file_with_full_path);
  }
}