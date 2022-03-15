// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

namespace netconf {

/**
 * Touches a file.
 * The file will be created if necessary.
 */
void TouchFile(::std::string& path);

}  // namespace netconf
