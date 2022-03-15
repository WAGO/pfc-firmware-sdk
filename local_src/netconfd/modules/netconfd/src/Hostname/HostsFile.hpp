// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "BaseTypes.hpp"

#include <vector>
#include <string>

namespace netconf {

void UpdateHostsFile(::std::vector<::std::string>& ips, const ::std::string &hostname, const ::std::string &domain);

::std::string UpdateFileContent(::std::string old_hosts, ::std::vector<::std::string>& ips,
                                const ::std::string &hostname, const ::std::string &domain);

} /* namespace netconf */

