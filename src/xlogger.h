#pragma once

#include <atomic>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>


#define V xlogger::Level::Verbose
#define D xlogger::Level::Debug
#define I xlogger::Level::Info
#define W xlogger::Level::Warning
#define E xlogger::Level::Error

#define XLOG xlogger::xlog

#define LOG_TAG ""

namespace xlogger {

// 强类型枚举，指定日志等级
enum class Level { Verbose = 0, Debug, Info, Warning, Error, Fatal };

class XLogger;  // 控制台输出用的日志类
class BaseLogger;      // 纯虚基类

class BaseLogger {
 protected:
  class CacheStream;  // 用于文本缓冲的内部类声明

 public:
  BaseLogger() = default;

  virtual ~BaseLogger() = default;

  // 重载 operator() 返回缓冲区对象
  virtual CacheStream operator()(Level log_level = Level::Debug);

 private:
  // 供缓冲区对象析构时调用（函数加锁保证线程安全）
  void endline(Level log_level, std::string &&log_string);

  // 纯虚函数，预留接口，由派生类实现
  virtual void write(Level log_level, std::string &log_string) = 0;

 private:
  std::mutex lock_;
};

// 用于文本缓冲区的类，继承 std::ostringstream
class BaseLogger::CacheStream : public std::ostringstream {
  BaseLogger &logger_;
  Level level_;

 public:
  // std::ios_base::ate: seek to the end of stream immediately after open
  CacheStream(BaseLogger &logger, Level log_level)
      : std::ostringstream(LOG_TAG, std::ios_base::ate),
        logger_(logger),
        level_(log_level){};

  CacheStream(const CacheStream &cache_stream)
      : std::ostringstream(LOG_TAG, std::ios_base::ate),
        logger_(cache_stream.logger_),
        level_(cache_stream.level_){};

  ~CacheStream()  // 为其重写析构函数，在析构时打日志
  {
    logger_.endline(level_, std::forward<std::string>(str()));
  }
};

class XLogger : public BaseLogger {
  virtual void write(Level log_level, std::string &log_string);
};

void set_level_threshold(Level threshold);

extern XLogger xlog;

}  // namespace xlogger
