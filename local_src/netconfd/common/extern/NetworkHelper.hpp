// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

namespace netconf {

::std::string GetPrefix(const ::std::string& netmask);

::std::string GetNetmask(const uint32_t& prefix);

::std::string GetBroadcast(const ::std::string& address, const ::std::string& netmask);

}
