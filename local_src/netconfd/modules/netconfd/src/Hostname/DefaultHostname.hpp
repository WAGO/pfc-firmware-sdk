// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace netconf {

::std::string GetDefaultHostname();

//only for internal testing
::std::string GetDefaultHostname(const ::std::string& board_variant, const ::std::string& base_mac);

} /* namespace netconf */

