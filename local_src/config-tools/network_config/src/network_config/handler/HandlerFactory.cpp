// SPDX-License-Identifier: GPL-2.0-or-later

#include "HandlerFactory.hpp"

#include <memory>

#include "BackupRestoreHandler.hpp"
#include "BridgeConfigHandler.hpp"
#include "DeviceInfoHandler.hpp"
#include "DipSwitchHandler.hpp"
#include "DSAModeHandler.hpp"
#include "FixIpHandler.hpp"
#include "InterfaceConfigHandler.hpp"
#include "IPConfigHandler.hpp"
#include "MacAddressHandler.hpp"
#include "OptionStrings.hpp"

namespace network_config {

::std::unique_ptr<IHandler> HandlerFactory::CreateHandler(const OptionParser &parser) {
  const auto &opts = GetOptions();
  const auto &map = parser.GetVariableMap();

  if (parser.IsSet(opts.bridge_config.name)) {
    return ::std::make_unique<BridgeConfigHandler>(map);

  } else if (parser.IsSet(opts.ip_config.name)) {
    return ::std::make_unique<IPConfigHandler>(parser);

  } else if (parser.IsSet(opts.mac_address.name)) {
    return ::std::make_unique<MacAddressHandler>(map);

  } else if (parser.IsSet(opts.interface_config.name)) {
    return ::std::make_unique<InterfaceConfigHandler>(map);

  } else if (parser.IsSet(opts.device_info.name)) {
    return ::std::make_unique<DeviceInfoHandler>(map);

  } else if (parser.IsSet(opts.backup.name)
      || parser.IsSet(opts.restore.name)
      || parser.IsSet(opts.get_backup_parameter_count.name)) {
    return ::std::make_unique<BackupRestoreHandler>(map);

  } else if (parser.IsSet(opts.dsa_mode.name)) {
    return ::std::make_unique<DSAModeHandler>(map);

  } else if (parser.IsSet(opts.fix_ip.name)) {
    return ::std::make_unique<FixIpHandler>();

  } else if (parser.IsSet(opts.dip_switch_config.name)) {
    return ::std::make_unique<DipSwitchHandler>(map);
  }

  return nullptr;
}

} /* namespace network_config */
