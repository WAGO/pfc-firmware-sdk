// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define DBOOST_LOG_DYN_LINK
//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <memory>
#include "Status.hpp"

namespace netconf {

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
//void LogStatus(Error const& status);

#define STR(x) #x
#define LogStatus(msg, status) LogStatus_(__FILE__, STR(__LINE__), msg, status)

void LogStatus_(::std::string const& file, ::std::string const& line, ::std::string const& message, Status const& status);


LogLevel LogLevelFromString(::std::string const& level);

}  // namespace netconf
