// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeManager.hpp"

#include <memory>

#include "BridgeConfigurator.hpp"
#include "BridgeConfigTransformator.hpp"
#include "IBridgeController.hpp"
#include "INetDevManager.hpp"
#include "IEventManager.hpp"
#include "IFileEditor.hpp"
#include "SwitchConfigLegacy.hpp"
#include "FileMonitor.hpp"

#include "IDeviceProperties.hpp"
#include "IBridgeInformation.hpp"

namespace netconf {

class BridgeManager : public IBridgeManager, public IBridgeInformation{
 public:
  BridgeManager(IBridgeController& bridge_controller, IDeviceProperties& properies_provider, INetDevManager& netdev_manager);
  virtual ~BridgeManager() = default;

  BridgeManager(const BridgeManager&) = delete;
  BridgeManager& operator=(const BridgeManager&) = delete;
  BridgeManager(const BridgeManager&&) = delete;
  BridgeManager& operator=(const BridgeManager&&) = delete;

  Bridges GetBridges() const override;
  BridgeConfig GetBridgeConfig() const override;
  Interfaces GetBridgeAssignedInterfaces() const override;
  Status ApplyBridgeConfiguration(BridgeConfig& product_config) const override;
  Status IsValid(BridgeConfig const& product_config) const override;

 private:
  IBridgeController& bridge_controller_;
  IDeviceProperties& properies_provider_;
  INetDevManager& netdev_manager_;
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

} /* namespace netconf */
