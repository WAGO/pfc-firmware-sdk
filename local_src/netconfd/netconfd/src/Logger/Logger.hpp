// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define DBOOST_LOG_DYN_LINK
//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <memory>

namespace netconfd {

enum class LogSink {
  TERMINAL,
  SYSLOG
};

/*
 * What is logged?
 * debug  --> logging: all
 * info   --> logging: info, warning, error
 * warning--> logging: warning, error
 * error  --> logging: error
 */
enum class LogLevel {
  debug,
  info,
  warning,
  error
};

void SetLogSink(LogSink sink);
void SetLogLevel(LogLevel level);

void LogWarning(::std::string const& message);
void LogInfo(::std::string const& message);
void LogError(::std::string const& message);
void LogDebug(::std::string const& message);

LogLevel LogLevelFromString(::std::string const& level);

}  // namespace netconfd
