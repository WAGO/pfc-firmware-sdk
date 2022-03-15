// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

namespace netconf {

std::string GetKernelHostname();

void SetKernelHostname(const std::string &hostname);

}
/* namespace netconf */

