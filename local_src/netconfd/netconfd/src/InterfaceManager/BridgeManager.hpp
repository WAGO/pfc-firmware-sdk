// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "BridgeConfigurator.hpp"
#include "BridgeConfigTransformator.hpp"
#include "IDevicePropertiesProvider.hpp"
#include "IBridgeController.hpp"
#include "JsonConfigConverter.hpp"
#include "IEventManager.hpp"
#include "MacDistributor.hpp"
#include "IFileEditor.hpp"
#include "SwitchConfigLegacy.hpp"
#include "FileMonitor.hpp"
#include <memory>

#include "BridgeConfigValidator.hpp"
#include "IBridgeManager.hpp"
#include "IInterfaceInformation.hpp"

namespace netconfd {

class BridgeManager : public IBridgeManager, public IInterfaceInformation{
 public:
  BridgeManager(IBridgeController& bridge_controller, IDevicePropertiesProvider& properies_provider);
  virtual ~BridgeManager() = default;

  BridgeManager(const BridgeManager&) = delete;
  BridgeManager& operator=(const BridgeManager&) = delete;
  BridgeManager(const BridgeManager&&) = delete;
  BridgeManager& operator=(const BridgeManager&&) = delete;

  Bridges GetBridges() const override;
  BridgeConfig GetBridgeConfig() const override;
  Interfaces GetInterfaces() const override;
  Interfaces GetBridgeAssignedInterfaces() const override;
  Bridge GetBridgeOfInterface(const Interface& itf) const override;
  bool HasAnyInterfacesUp(const Bridge& bridge) const override;
  Status Configure(const BridgeConfig& product_config) const override;
  Status IsValid(BridgeConfig const& product_config) const override;
  Status SetBridgeUp(const Bridge& bridge) const override;
  Status SetBridgeDown(const Bridge& bridge) const override;
  bool IsBridge(const Interface& itf) const override;
  bool IsInterfaceUp(const Interface& itf) const override;

 private:
  IBridgeController& bridge_controller_;
  IDevicePropertiesProvider& properies_provider_;
  MacDistributor mac_distributor_;
  BridgeConfigValidator interface_validator_;
  BridgeConfigurator bridge_configurator_;
  BridgeConfigTransformator bridge_config_transformator_;
  SwitchConfigLegacy switch_config_;
  FileMonitor switch_settings_file_monitor_;


  void UpdateAgetime() const;
  Status SetDefaultInterfaceUp() const;
  Status PrepareBridgeConfig(BridgeConfig const& product_config,
                             BridgeConfig& os_config) const;
  Status ApplyBridgeConfig(BridgeConfig const& os_config) const;
};

} /* namespace netconfd */
