// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

struct nl_sock;
struct nl_cache_mngr;

namespace netconf {


class NetlinkCache {
 public:
  NetlinkCache()                    = default;
  NetlinkCache(const NetlinkCache&) = delete;
  NetlinkCache(NetlinkCache&&)      = delete;
  virtual ~NetlinkCache()           = default;

  virtual void Resync(struct nl_sock*) = 0;

};

}  // namespace netconf