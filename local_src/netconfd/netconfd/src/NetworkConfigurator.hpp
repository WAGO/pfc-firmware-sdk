// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#include <memory>

namespace netconfd {

class NetworkConfiguratorImpl;

class NetworkConfigurator {
 public:
  NetworkConfigurator();
  virtual ~NetworkConfigurator();

  NetworkConfigurator(const NetworkConfigurator&) = delete;
  NetworkConfigurator& operator=(const NetworkConfigurator&) = delete;
  NetworkConfigurator(const NetworkConfigurator&&) = delete;
  NetworkConfigurator& operator=(const NetworkConfigurator&&) = delete;

 private:
  ::std::unique_ptr<NetworkConfiguratorImpl> network_configurator_;
};

}  // namespace netconfd
