// SPDX-License-Identifier: GPL-2.0-or-later
#include "TypesHelper.hpp"
#include "IpConfigHelper.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "Helper.hpp"

namespace netconf {

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

}  // namespace netconf
