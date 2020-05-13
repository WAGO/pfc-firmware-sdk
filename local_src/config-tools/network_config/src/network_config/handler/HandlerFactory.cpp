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

namespace network_config
{

  ::std::unique_ptr<IHandler> HandlerFactory::CreateHandler(const OptionParser& parser)
  {
    const auto &optstr = GetOptions();
    const auto& vm = parser.GetVariableMap();

    if(vm.count(optstr.brigde_config.name) > 0)
    {
      return ::std::make_unique < BridgeConfigHandler > (vm);
    }
    else if(vm.count(optstr.ip_config.name) > 0)
    {
      return ::std::make_unique < IPConfigHandler > (parser);
    }
    else if(vm.count(optstr.mac_address.name) > 0)
    {
      return ::std::make_unique < MacAddressHandler > (vm);
    }
    else if(vm.count(optstr.interface_config.name) > 0)
    {
      return ::std::make_unique < InterfaceConfigHandler > (vm);
    }
    else if(vm.count(optstr.device_info.name) > 0)
    {
      return ::std::make_unique < DeviceInfoHandler > (vm);
    }
    else if(vm.count(optstr.backup.name)  > 0 || vm.count(optstr.restore.name)  > 0 || vm.count(optstr.get_backup_parameter_count.name)  > 0 )
    {
      return ::std::make_unique < BackupRestoreHandler > (vm);
    }
    else if(vm.count(optstr.dsa_mode.name) > 0)
    {
      return ::std::make_unique < DSAModeHandler > (vm);
    }
    else if(vm.count(optstr.fix_ip.name) > 0)
    {
      return ::std::make_unique < FixIpHandler > ();
    }
    else if(vm.count(optstr.dip_switch_config.name) > 0)
    {
      return ::std::make_unique<DipSwitchHandler>(vm);
    }


    return nullptr;
  }

} /* namespace network_config */
