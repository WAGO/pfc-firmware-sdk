// SPDX-License-Identifier: LGPL-3.0-or-later
//------------------------------------------------------------------------------
#pragma once

namespace netconf {

enum class DynamicIPEventAction {
  UNKNOWN,
  BOUND,
  RELEASE,
  RENEW,
  NAK
};

}

