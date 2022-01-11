// SPDX-License-Identifier: LGPL-3.0-or-later
//------------------------------------------------------------------------------
#pragma once


#include <utility>
#include <map>
#include <string>
#include <vector>

#include "BaseTypes.hpp"
#include "DeviceType.hpp"
#include "InterfaceInformation.hpp"
#include "MacAddress.hpp"

namespace netconf {


using Interfaces = ::std::vector<Interface>;
using Bridges = ::std::vector<Bridge>;
using BridgePair = std::pair<Bridge, Interfaces>;
using BridgeConfig = ::std::map<Bridge, Interfaces>;

using InterfaceNamesPair = std::pair<Interface, Interface>;
using InterfaceNameMapping = ::std::map<Interface, Interface>;

using IPConfigs = ::std::vector<IPConfig>;
using InterfaceConfigs = ::std::vector<InterfaceConfig>;
using InterfaceStatuses = ::std::vector<InterfaceStatus>;

}

