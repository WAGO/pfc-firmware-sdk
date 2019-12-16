// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPManager.hpp"

namespace netconfd {

IPManager::IPManager(const IDevicePropertiesProvider& properties_provider,
                     const IInterfaceInformation& interface_manager)
    : properties_provider_ { properties_provider },
      interface_manager_ { interface_manager },
      dhcp_client_controller_ { command_executor_, properties_provider_, file_editor_ },
      ip_configurator_ { ip_controller_, dhcp_client_controller_, bootp_controller_, interface_manager_ },
      ip_validator_ { ip_controller_ } {

}

Status IPManager::Configure(const IPConfigs& configs) const {

  return ip_configurator_.Configure(configs);

}

IPConfigs IPManager::GetIPConfigs(const Bridges& bridges) const {

  return ip_configurator_.GetConfigurations(bridges);
}

bool IPManager::IsApplicableToSystem(const IPConfigs& configs) const {

  Interfaces not_assignable_interfaces = interface_manager_.GetBridgeAssignedInterfaces();
  Interfaces system_interfaces = interface_manager_.GetInterfaces();

  return ip_validator_.IsInterfaceApplicableToSystem(configs, system_interfaces, not_assignable_interfaces);
}

Status IPManager::ValidateIPConfigs(const IPConfigs& configs,
                                    const bool interference_has_to_be_checked) const {

  return ip_validator_.ValidateIPConfigs(configs, interference_has_to_be_checked);
}

Status IPManager::ValidateIPConfigIsApplicableToSystem(const IPConfigs& configs) const {

  Interfaces system_interfaces = interface_manager_.GetInterfaces();

  return ip_validator_.ValidateIPConfigIsApplicableToSystem(configs, system_interfaces);
}

} /* namespace netconfd */
