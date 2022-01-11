// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "LinkMode.hpp"
#include <gsl/gsl>

namespace netconf {

LinkModes ToLinkModes(gsl::span<const uint32_t> linkmodes);


} /* namespace netconf */

