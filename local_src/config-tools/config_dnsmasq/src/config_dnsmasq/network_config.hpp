//------------------------------------------------------------------------------
/// Copyright (c) 2020-2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Reading of network information (ethernet, ip and interfaces).
///
///  \author   MSc, WAGO GmbH & Co. KG.
///  \author   MOe, WAGO GmbH & Co. KG.
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_
#define SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_

#include <string>
#include <vector>

#include "defines.hpp"
#include "IpConfiguration.hpp"

namespace configdnsmasq {

void netcfg_read_settings(IpConfiguration &data, std::vector<std::string> &legal_ports,
                          const prgconf_t &prgconf, int debugmode);
}

#endif /* SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_ */
