// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "IBridgeController.hpp"

namespace netconf {

class BridgeController : public IBridgeController{
 public:
  explicit BridgeController();
  ~BridgeController();

  BridgeController(const BridgeController&) = default;
  BridgeController& operator=(const BridgeController&) = default;
  BridgeController(const BridgeController&&) = delete;
  BridgeController& operator=(const BridgeController&&) = delete;

  Status AddBridge(const Bridge& bridge) const override;
  Status DeleteBridge(const Bridge& bridge) const override;
  Status AddInterface(const Bridge& bridge, const Interface& interface) const override;
  Status DeleteInterface(const Bridge& bridge, const Interface& interface) const override;

  Bridges GetBridges() const override;
  Interfaces GetBridgeInterfaces(const Bridge& bridge) const override;

  Interfaces GetInterfaces() const override;


  Status SetInterfaceUp(const ::std::string& name) const override;
  Status SetInterfaceDown(const ::std::string& name) const override;

  void SetAgetime(const Bridge& bridge, int seconds) const override;
};


}  // namespace netconf
