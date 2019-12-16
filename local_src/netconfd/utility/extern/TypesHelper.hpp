// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     TypesConverte.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef UTILITY_EXTERN_TYPESCONVERTE_HPP_
#define UTILITY_EXTERN_TYPESCONVERTE_HPP_

#include <string>

#include "Types.hpp"

namespace netconfd {

::std::string IPSourceToString(const IPSource value);
IPSource StringToIPSource(const ::std::string& value);

bool IsEqual (const BridgeConfig& a, const BridgeConfig& b);
bool IsEqual(IPConfigs& lhs, IPConfigs& rhs);

void AddIPConfig(const IPConfig& ip_config, IPConfigs& ip_configs);

Bridges GetBridgesOfBridgeConfig(const BridgeConfig& bridge_config);

bool ConfigContainsBridgeWithSpecificInterfaces(const BridgeConfig& bridge_config,
                                                const Bridge& bridge,
                                                const Interfaces& interfaces);


void IpConfigsIntersection(IPConfigs& ip_configs, const Bridges& selection);

IPConfig& ComplementNetmask(IPConfig& ip_config);
IPConfigs& ComplementNetmasks(IPConfigs& ip_configs);


}  // namespace name

#endif /* UTILITY_EXTERN_TYPESCONVERTE_HPP_ */
//---- End of source file ------------------------------------------------------

