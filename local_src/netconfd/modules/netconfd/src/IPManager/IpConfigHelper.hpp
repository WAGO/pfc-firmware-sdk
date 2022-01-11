// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IpConfigHelper.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include "Types.hpp"
#include "NetDev.hpp"
#include "IDipSwitch.hpp"

namespace netconf {

void CleanWithRespectToSystem(IPConfigs &ip_configs, const NetDevs &netdevs,
                              const Interfaces &not_assignable_interfaces);


IPConfig CreateIpConfigFromDipInfos(const DipSwitchIpConfig& dipIpConfig, const DipSwitchState& state);

void ModifyIpConfigByDipSwitch(IPConfigs &config, const DipSwitchIpConfig &dip_switch_config, const DipSwitchState dipSwitchState);

void RemoveIpConfigsWithConflicsTo(const IPConfig& testee, IPConfigs& fromConfigs, IPSource havingSource = IPSource::STATIC);
}  // namespace netconf

//---- End of header file ------------------------------------------------------

