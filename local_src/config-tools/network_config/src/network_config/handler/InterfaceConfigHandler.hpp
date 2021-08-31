// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

#include "InterfaceConfig.hpp"

namespace network_config {

class InterfaceConfigHandler : public IHandler {
 public:
  explicit InterfaceConfigHandler(const ::boost::program_options::variables_map &vm);
  ~InterfaceConfigHandler() override = default;

  InterfaceConfigHandler(const InterfaceConfigHandler&) = delete;
  InterfaceConfigHandler& operator=(const InterfaceConfigHandler&) = delete;
  InterfaceConfigHandler(const InterfaceConfigHandler&&) = delete;
  InterfaceConfigHandler& operator=(const InterfaceConfigHandler&&) = delete;

  void Execute() override;
 private:
  void GetConfig();
  void SetConfig();
  netconf::api::InterfaceConfigs CreateInterfaceConfigs();

  ::std::function<void()> execute_;
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
