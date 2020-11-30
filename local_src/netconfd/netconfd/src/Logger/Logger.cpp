// SPDX-License-Identifier: GPL-2.0-or-later

#include "Logger.hpp"

#include <memory>
#include <iostream>
#define BOOST_LOG_USE_NATIVE_SYSLOG 1
//#include <boost/log/detail/config.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>

namespace netconf {

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

class Logger {
 public:
  explicit Logger() = default;
  ~Logger() = default;

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(const Logger&&) = delete;
  Logger& operator=(const Logger&&) = delete;

  void LogDebug(const ::std::string& message);
  void LogInfo(const ::std::string& message);
  void LogWarning(const ::std::string& message);
  void LogError(const ::std::string& message);

  void SetLogLevel(LogLevel level);
  void SetLogSink(LogSink sink);

 private:
  void EnableSyslog();
  void DisableSyslog();

  ::boost::shared_ptr<sinks::synchronous_sink<sinks::syslog_backend>> syslog_sink_;

};

void Logger::SetLogLevel(LogLevel level) {
  switch (level) {
    case LogLevel::debug:
      logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
      break;
    case LogLevel::info:
      logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
      break;
    case LogLevel::warning:
      logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::warning);
      break;
    case LogLevel::error:
      logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::error);
      break;
    default:
      break;
  }
}

void Logger::SetLogSink(LogSink sink) {

  switch (sink) {
    case LogSink::TERMINAL:
      DisableSyslog();
      break;
    case LogSink::SYSLOG:
      EnableSyslog();
      break;
    default:
      break;
  }
}

void Logger::DisableSyslog() {
  if (syslog_sink_) {
    boost::shared_ptr<logging::core> core = logging::core::get();
    core->remove_sink(syslog_sink_);
  }
}

void Logger::EnableSyslog() {

  boost::shared_ptr<logging::core> core = logging::core::get();

  boost::shared_ptr<sinks::synchronous_sink<sinks::syslog_backend> > sink(
      new sinks::synchronous_sink<sinks::syslog_backend>(keywords::use_impl = sinks::syslog::native,
                                                         keywords::facility = sinks::syslog::user));

  syslog_sink_ = sink;

  sinks::syslog::custom_severity_mapping<std::string> mapping("Severity");
  mapping["debug"] = sinks::syslog::debug;
  mapping["info"] = sinks::syslog::info;
  mapping["warning"] = sinks::syslog::warning;
  mapping["error"] = sinks::syslog::error;
  syslog_sink_->locked_backend()->set_severity_mapper(mapping);

  core->add_sink(syslog_sink_);
}

void Logger::LogDebug(const ::std::string& message) {
  BOOST_LOG_TRIVIAL(debug)<< "DEBUG: " << message;
}

void Logger::LogInfo(const ::std::string& message) {
  BOOST_LOG_TRIVIAL(info)<< "INFO: " << message;
}

void Logger::LogWarning(const ::std::string& message) {
  BOOST_LOG_TRIVIAL(warning)<< "WARNING: " << message;
}

void Logger::LogError(const ::std::string& message) {
  BOOST_LOG_TRIVIAL(error)<<"ERROR: " << message;
}

static Logger& GetLogger() {
  static Logger logger;
  return logger;
}

void SetLogSink(LogSink sink) {
  GetLogger().SetLogSink(sink);
}

void SetLogLevel(LogLevel level) {
  GetLogger().SetLogLevel(level);
}

void LogWarning(::std::string const& message) {
  GetLogger().LogWarning(message);
}

void LogInfo(::std::string const& message) {
  GetLogger().LogInfo(message);
}

void LogError(::std::string const& message) {
  GetLogger().LogError(message);
}

void LogDebug(::std::string const& message) {
  GetLogger().LogDebug(message);
}

void LogStatus_(::std::string const& file, ::std::string const& line, ::std::string const& message, Error const& status) {
  if (status.IsNotOk()) {
    auto msg = file +":" + line + ": " + message + ": " + status.ToString();
    LogDebug(msg);
  }
}


LogLevel LogLevelFromString(::std::string const& level) {

  LogLevel log_level = {};
  if (level == "debug") {
    log_level = LogLevel::debug;
  } else if (level == "info") {
    log_level = LogLevel::info;
  } else if (level == "warning") {
    log_level = LogLevel::warning;
  } else if (level == "error") {
    log_level = LogLevel::error;
  } else {
    log_level = LogLevel::warning;
  }
  return log_level;
}


}
// namespace netconf

//---- End of source file ------------------------------------------------------
