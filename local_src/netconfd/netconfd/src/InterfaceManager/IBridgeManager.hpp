// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
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
  virtual Status ApplyBridgeConfiguration(BridgeConfig& product_config) const = 0;
  virtual Status IsValid(BridgeConfig const& product_config) const = 0;
  virtual Status SetBridgeUp(const Bridge& bridge) const = 0;
  virtual Status SetBridgeDown(const Bridge& bridge) const = 0;

};

} /* namespace netconf */
