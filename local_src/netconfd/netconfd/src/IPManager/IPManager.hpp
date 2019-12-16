// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IInterfaceInformation.hpp"
#include "BootpClientController.hpp"
#include "IIPManager.hpp"
#include "IPConfigurator.hpp"
#include "IPController.hpp"
#include "DHCPClientController.hpp"
#include "IPValidator.hpp"
#include "IDevicePropertiesProvider.hpp"
#include "FileEditor.hpp"
#include "CommandExecutor.hpp"

namespace netconfd {

class IPManager : public IIPManager {
 public:
  IPManager(const IDevicePropertiesProvider& properties_provider,
            const IInterfaceInformation& interface_manager);
  virtual ~IPManager() = default;

  IPManager(const IPManager&) = delete;
  IPManager& operator=(const IPManager&) = delete;
  IPManager(const IPManager&&) = delete;
  IPManager& operator=(const IPManager&&) = delete;

  Status Configure(const IPConfigs& config) const override;
  bool IsApplicableToSystem(const IPConfigs& configs) const override;
  Status ValidateIPConfigs(const IPConfigs& configs,
                           const bool interference_has_to_be_checked) const override;
  Status ValidateIPConfigIsApplicableToSystem(const IPConfigs& configs) const override;


  IPConfigs GetIPConfigs(const Bridges& bridges) const override;

 private:

  IPConfigs GetAllIpConfigs(const Interfaces& interfaces) const;
  Status CompletePartialIPConfig(IPConfigs & configs) const;

  const IDevicePropertiesProvider& properties_provider_;
  const IInterfaceInformation& interface_manager_;
  FileEditor file_editor_;
  CommandExecutor command_executor_;
  DHCPClientController dhcp_client_controller_;
  BootpClientController bootp_controller_;
  IPController ip_controller_;
  IPConfigurator ip_configurator_;
  IPValidator ip_validator_;

};

} /* namespace netconfd */
