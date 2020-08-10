// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#include <memory>
#include "InterprocessCondition.h"

namespace netconf {

class NetworkConfiguratorImpl;

class NetworkConfigurator {
 public:
  NetworkConfigurator(InterprocessCondition& start_condition);
  virtual ~NetworkConfigurator();

  NetworkConfigurator(const NetworkConfigurator&) = delete;
  NetworkConfigurator& operator=(const NetworkConfigurator&) = delete;
  NetworkConfigurator(const NetworkConfigurator&&) = delete;
  NetworkConfigurator& operator=(const NetworkConfigurator&&) = delete;

 private:
  ::std::unique_ptr<NetworkConfiguratorImpl> network_configurator_;
};

}  // namespace netconf
