// SPDX-License-Identifier: GPL-2.0-or-later
#include "TypesHelper.hpp"
#include "IpConfigHelper.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "Helper.hpp"
#include "IpAddressManipulator.hpp"

#include "IPValidator.hpp"

namespace netconf {

namespace {

inline const DipSwitchIpConfig& getDefaultDipSwitchIpConfig() {
  static const DipSwitchIpConfig DIP_SWITCH_DEFAULT_IP_CONFIG = DipSwitchIpConfig("192.168.1.0", "255.255.255.0");
  return DIP_SWITCH_DEFAULT_IP_CONFIG;
}

inline bool HasToApplyDipSwitchConfig(DipSwitchMode mode) {
  return (mode != DipSwitchMode::HW_NOT_AVAILABLE && mode != DipSwitchMode::OFF);
}

}

void CleanWithRespectToSystem(IPConfigs &ip_configs, const NetDevs &netdevs,
                              const Interfaces &not_assignable_interfaces) {

  auto first_deleted_it = ::std::remove_if(ip_configs.begin(), ip_configs.end(), [&](auto &ip_config) {
    const Interface &interface = ip_config.interface_;
    if (NetDevManager::DoesNotExistByName(interface, netdevs) || IsIncluded(interface, not_assignable_interfaces)) {
      return true;
    }
    return false;
  });

  std::for_each(first_deleted_it, ip_configs.end(), [](auto &deleted_ipcfg) {
    LogWarning("Drop not applicable IP config: " + IPConfigToString(deleted_ipcfg));
  });

  ip_configs.erase(first_deleted_it, ip_configs.end());
}

IPConfig CreateIpConfigFromDipInfos(const DipSwitchIpConfig &dipIpConfig, const DipSwitchState &state) {

  IPConfig ipConfig { DIP_SWITCH_BRIDGE };
  if (state.mode == DipSwitchMode::DHCP) {
    ipConfig.source_ = IPSource::DHCP;
  } else {
    if (dipIpConfig.IsIncomplete()) {
      ipConfig.address_ = getDefaultDipSwitchIpConfig().address_;
      ipConfig.netmask_ = getDefaultDipSwitchIpConfig().netmask_;
    } else {
      ipConfig.address_ = IpAddressManipulator::ChangeLastAddressOctet(dipIpConfig.address_, state.value);
      ipConfig.netmask_ = dipIpConfig.netmask_;
    }
    ipConfig.source_ = IPSource::STATIC;
  }
  return ipConfig;
}

void ModifyIpConfigByDipSwitch(IPConfigs &configs, const DipSwitchIpConfig &dip_switch_config,
                               const DipSwitchState dipSwitchState) {
  if (!HasToApplyDipSwitchConfig(dipSwitchState.mode)) {
    return;
  }

  auto itDipIpConfig = GetIpConfigByInterface(configs, DIP_SWITCH_BRIDGE);
  if (itDipIpConfig == configs.end()) {
    return;
  }


  if (itDipIpConfig->source_ == IPSource::TEMPORARY && dipSwitchState.mode == DipSwitchMode::DHCP) {
    // Dont touch the DHCP IP request.
    return;
  }

  *itDipIpConfig = CreateIpConfigFromDipInfos(dip_switch_config, dipSwitchState);

}


void RemoveIpConfigsWithConflicsTo(const IPConfig& testee, IPConfigs& fromConfigs, IPSource havingSource)
{
  // remove conflicting ip configs on other interfaces
  fromConfigs.erase(
      ::std::remove_if(
          fromConfigs.begin(),
          fromConfigs.end(),
          [&](const IPConfig& ipconfig) {
            return ipconfig.interface_ != testee.interface_ && ipconfig.source_ == havingSource
                && IPValidator::IsSameSubnet(ipconfig, testee);
          })
      , fromConfigs.end());
}

}  // namespace netconf
