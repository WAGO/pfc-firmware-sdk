// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeConfigurator.hpp"
#include "IBridgeController.hpp"
#include "IMacDistributor.hpp"
#include "Types.hpp"
#include "FileEditor.hpp"
#include <memory>


//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

namespace netconf {

class BridgeConfigurator : public IBridgeConfigurator {

 public:
  explicit BridgeConfigurator(IBridgeController& bridge_controller);
  ~BridgeConfigurator() override = default;

  BridgeConfigurator(const BridgeConfigurator&) = delete;
  BridgeConfigurator& operator=(const BridgeConfigurator&) = delete;
  BridgeConfigurator(const BridgeConfigurator&&) = delete;
  BridgeConfigurator& operator=(const BridgeConfigurator&&) = delete;

  BridgeConfig GetConfiguration() const override;
  Status Configure(BridgeConfig const& config) const override;

  Interfaces GetBridgeAssignedInterfaces() const;

 private:

  Status RemoveAllActualBridgesThatAreNotNeeded(
      BridgeConfig const& config_os, Bridges& adapted_target_bridges) const;
  Status RemoveAllActualBridgeInterfacesThatAreNotNeeded(Bridges const& actual_bridges,
                                                       BridgeConfig const& config_os) const;
  Status AddMissingInterfacesToActualBridges(Bridges const& actual_bridges,
                                           BridgeConfig const& config_os) const;
  Status AddMissingBridgesAndTheirInterfaces(Bridges const& actual_bridges,
                                           BridgeConfig const& config_os) const;
  Status SetAllBridgesUp(BridgeConfig const &config_os) const;
  Status SetInterfaceDown(Interface const& interface) const;
  Status SetInterfaceUpAndAddToBridge(Interface const& interface, Bridge const& bridge) const;
  Status SetInterfaceDownAndDeleteFromBridge(Interface const& interface,
                                             Bridge const& bridge) const;
  Status AddBridge(Bridge const& bridge) const;
  Status SetBridgeDownAndDelete(Bridge const& bridge) const;
  Status SetDefaultInterfaceUp() const;

  IBridgeController& bridge_controller_;

};

}  // namespace netconf
