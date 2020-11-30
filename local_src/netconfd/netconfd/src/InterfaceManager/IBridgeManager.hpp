// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Error.hpp"
#include "Types.hpp"

namespace netconf {

class IBridgeManager {
 public:
  IBridgeManager() = default;
  virtual ~IBridgeManager() = default;

  IBridgeManager(const IBridgeManager&) = delete;
  IBridgeManager& operator=(const IBridgeManager&) = delete;
  IBridgeManager(const IBridgeManager&&) = delete;
  IBridgeManager& operator=(const IBridgeManager&&) = delete;

  virtual Bridges GetBridges() const = 0;
  virtual BridgeConfig GetBridgeConfig() const = 0;
  virtual Error ApplyBridgeConfiguration(BridgeConfig& product_config) const = 0;
  virtual Error IsValid(BridgeConfig const& product_config) const = 0;

};

} /* namespace netconf */
