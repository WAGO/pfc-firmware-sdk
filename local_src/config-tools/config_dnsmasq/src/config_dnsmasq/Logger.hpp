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

#ifndef SRC_CONFIG_DNSMASQ_LOGGER_HPP_
#define SRC_CONFIG_DNSMASQ_LOGGER_HPP_

#include <string>

namespace configdnsmasq
{

  class Logger
  {
    public:
      Logger(::std::string const& identifier);
      virtual ~Logger();
      Logger(const Logger &other) = default;
      Logger(Logger &&other) = default;
      Logger& operator=(const Logger &other) = delete;
      Logger& operator=(Logger &&other) = delete;

      ::std::string identifier_;
  };

} /* namespace configdnsmasq */

#endif /* SRC_CONFIG_DNSMASQ_LOGGER_HPP_ */
