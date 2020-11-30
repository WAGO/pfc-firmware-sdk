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
  Error Configure(BridgeConfig const& config) const override;

  Interfaces GetBridgeAssignedInterfaces() const;

 private:

  Error RemoveAllActualBridgesThatAreNotNeeded(
      BridgeConfig const& config_os, Bridges& adapted_target_bridges) const;
  Error RemoveAllActualBridgeInterfacesThatAreNotNeeded(Bridges const& actual_bridges,
                                                       BridgeConfig const& config_os) const;
  Error AddMissingInterfacesToActualBridges(Bridges const& actual_bridges,
                                           BridgeConfig const& config_os) const;
  Error AddMissingBridgesAndTheirInterfaces(Bridges const& actual_bridges,
                                           BridgeConfig const& config_os) const;
  Error SetAllBridgesUp(BridgeConfig const &config_os) const;
  Error SetInterfaceDown(Interface const& interface) const;
  Error SetInterfaceUpAndAddToBridge(Interface const& interface, Bridge const& bridge) const;
  Error SetInterfaceDownAndDeleteFromBridge(Interface const& interface,
                                             Bridge const& bridge) const;
  Error AddBridge(Bridge const& bridge) const;
  Error SetBridgeDownAndDelete(Bridge const& bridge) const;
  Error SetDefaultInterfaceUp() const;

  IBridgeController& bridge_controller_;

};

}  // namespace netconf
