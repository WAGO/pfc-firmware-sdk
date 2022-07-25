//------------------------------------------------------------------------------
/// Copyright (c) 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Write logging data.
///
///  \author   MSc, WAGO GmbH & Co. KG.
///  \author   MOe, WAGO GmbH & Co. KG.
//------------------------------------------------------------------------------

#include "Logger.hpp"

#include <syslog.h>

namespace configdnsmasq
{

  Logger::Logger(::std::string const& identifier)
  {
    identifier_ = identifier;
    openlog(identifier.c_str(), LOG_CONS | LOG_NDELAY, LOG_SYSLOG);
  }

  Logger::~Logger()
  {
    closelog();
  }

} /* namespace configdnsmasq */
