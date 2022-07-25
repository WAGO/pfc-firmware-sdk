// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define DBOOST_LOG_DYN_LINK
//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <memory>

#include "Status.hpp"

namespace netconf {

enum class LogSink { TERMINAL, SYSLOG };

/*
 * What is logged?
 * debug  --> logging: all
 * info   --> logging: info, warning, error
 * warning--> logging: warning, error
 * error  --> logging: error
 */
enum class LogLevel { debug, info, warning, error };

void SetLogSink(LogSink sink);
void SetLogLevel(LogLevel level);

void LogWarning(::std::string const& message);
void LogInfo(::std::string const& message);
void LogError(::std::string const& message);
void LogDebug(::std::string const& message);

static inline void LogDebug(::std::string const& func, ::std::string const& message) {
  LogDebug(func + ": " + message);
}

#define LOG_DEBUG(message)            \
  do {                                \
    ::std::string func{&__func__[0]}; \
    LogDebug(func, message);          \
  } while (0)

LogLevel LogLevelFromString(::std::string const& level);

}  // namespace netconf
