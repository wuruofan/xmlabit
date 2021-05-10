#include "xlogger.h"
#include <map>
#include <iostream>

namespace xlogger {

XLogger xlog;

std::atomic<Level> g_level_threshold = {Level::Debug};

static const std::map<Level, const char*> level_string_mini_map = {
    {Level::Debug, "D"},     {Level::Info, "I"},
    {Level::Warning, "W"}, {Level::Error, "E"},
    {Level::Fatal, "F"},
};

static const std::map<Level, const char*> level_string_map = {
    {Level::Debug, "Debug"},     {Level::Info, "Info"},
    {Level::Warning, "Warning"}, {Level::Error, "Error"},
    {Level::Fatal, "Fatal"},
};

BaseLogger::CacheStream BaseLogger::operator()(Level log_lvl) {
  return CacheStream(*this, log_lvl);
}

void BaseLogger::endline(Level log_level, std::string&& log_string) {
  lock_.lock();
  write(log_level, log_string);
  lock_.unlock();
}

void XLogger::write(Level log_level, std::string& log_string) {
  if (log_level >= g_level_threshold) {
    // std::cout << '[' << level_string_map.find(log_level)->second << "]"
         // << "\t" << log_string << std::endl;
    // std::cout << level_string_mini_map.find(log_level)->second << "| "
          // << log_string << std::endl;
    std::cout << log_string << std::endl;
    std::cout.flush();
  }
}

void set_level_threshold(Level threshold) {
  if (threshold < Level::Debug || threshold > Level::Fatal) {
    xlog(E) << "[" << __FUNCTION__ << "] unsupported log level!";
    return;
  } else {
    g_level_threshold = threshold;
    xlog(D) << "[" << __FUNCTION__ << "] logger level threshold set: "
             << level_string_map.find(threshold)->second << " !";
  }
}

}  // namespace xlogger
