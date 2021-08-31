// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>
#include "DataFieldAccess.hpp"

namespace network_config {

class DeviceInfoHandler : public IHandler {
 public:
  explicit DeviceInfoHandler(const ::boost::program_options::variables_map &vm);
  ~DeviceInfoHandler() override = default;

  DeviceInfoHandler(const DeviceInfoHandler&) = delete;
  DeviceInfoHandler& operator=(const DeviceInfoHandler&) = delete;
  DeviceInfoHandler(const DeviceInfoHandler&&) = delete;
  DeviceInfoHandler& operator=(const DeviceInfoHandler&&) = delete;

  void Execute() override;

 private:
  const ::boost::program_options::variables_map &vm_;
  DataFieldAccess<::netconf::InterfaceInformation> dfa_;
};

} /* namespace network_config */
