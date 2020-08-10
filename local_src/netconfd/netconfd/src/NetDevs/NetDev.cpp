// SPDX-License-Identifier: GPL-2.0-or-later

extern "C" {
#include <sys/socket.h>
#include <linux/if.h>
}

#include "NetDev.hpp"
#include <algorithm>

namespace netconf {

void NetDev::SetIfFlags(::std::uint32_t flags) {

  auto changed = flags_ xor flags;
  flags_ = flags;

  if ((changed & IFF_LOWER_UP) == 0) {
    return;
  }

  link_state_ = (flags & IFF_LOWER_UP) == IFF_LOWER_UP ? eth::InterfaceLinkState::Up : eth::InterfaceLinkState::Down;

  if (onlinkchange_) {
    onlinkchange_(*this, link_state_);
  }

}

}  // namespace netconf
