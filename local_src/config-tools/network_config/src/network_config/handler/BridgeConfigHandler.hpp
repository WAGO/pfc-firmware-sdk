// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

namespace network_config {

class BridgeConfigHandler : public IHandler {
 public:
  explicit BridgeConfigHandler(const ::boost::program_options::variables_map &vm);
  ~BridgeConfigHandler() override = default;

  BridgeConfigHandler(const BridgeConfigHandler&) = delete;
  BridgeConfigHandler& operator=(const BridgeConfigHandler&) = delete;
  BridgeConfigHandler(const BridgeConfigHandler&&) = delete;
  BridgeConfigHandler& operator=(const BridgeConfigHandler&&) = delete;

  void Execute() override;
 private:
  void GetConfig();
  void SetConfig();

  ::std::function<void()> execute_;
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
