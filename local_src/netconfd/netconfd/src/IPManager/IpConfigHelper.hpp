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

namespace netconfd {

void CleanWithRespectToSystem(IPConfigs &ip_configs, const NetDevs &netdevs,
                              const Interfaces &not_assignable_interfaces);


}  // namespace netconfd

//---- End of header file ------------------------------------------------------

