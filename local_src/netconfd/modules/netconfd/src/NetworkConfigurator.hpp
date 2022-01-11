// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#include <memory>
#include "InterprocessCondition.h"
#include "NetworkConfiguratorSettings.hpp"

namespace netconf {

class NetworkConfiguratorImpl;

class NetworkConfigurator {

 public:
  NetworkConfigurator(InterprocessCondition& start_condition, StartWithPortstate startWithPortState = StartWithPortstate::Normal);
  virtual ~NetworkConfigurator();

  NetworkConfigurator(const NetworkConfigurator&) = delete;
  NetworkConfigurator& operator=(const NetworkConfigurator&) = delete;
  NetworkConfigurator(const NetworkConfigurator&&) = delete;
  NetworkConfigurator& operator=(const NetworkConfigurator&&) = delete;

 private:
  ::std::unique_ptr<NetworkConfiguratorImpl> network_configurator_;
};

}  // namespace netconf
