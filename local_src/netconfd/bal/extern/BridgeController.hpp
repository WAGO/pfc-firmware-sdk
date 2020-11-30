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

  Error AddBridge(const Bridge& bridge) const override;
  Error DeleteBridge(const Bridge& bridge) const override;
  Error AddInterface(const Bridge& bridge, const Interface& interface) const override;
  Error DeleteInterface(const Bridge& bridge, const Interface& interface) const override;

  Bridges GetBridges() const override;
  Interfaces GetBridgeInterfaces(const Bridge& bridge) const override;

  Interfaces GetInterfaces() const override;


  Error SetInterfaceUp(const ::std::string& name) const override;
  Error SetInterfaceDown(const ::std::string& name) const override;

  void SetAgetime(const Bridge& bridge, int seconds) const override;
};


}  // namespace netconf
