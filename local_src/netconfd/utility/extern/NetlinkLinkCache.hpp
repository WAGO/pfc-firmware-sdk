// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "NetlinkCache.hpp"

#include <memory>

#include "IInterfaceMonitor.hpp"

namespace netconf {

class NetlinkLinkCache : public NetlinkCache, public IInterfaceMonitor {
 public:
  NetlinkLinkCache() = delete;
  NetlinkLinkCache(nl_sock* nl_sock, nl_cache_mngr* nl_cache_mgr);
  ~NetlinkLinkCache();

  void Resync(nl_sock*) override;

  void RegisterEventHandler(IInterfaceEvent& event_handler) override;
  void UnregisterEventHandler(IInterfaceEvent& event_handler) override;

  ::std::uint32_t GetIffFlags(::std::uint32_t if_index) override;
  ::std::int32_t GetAddressFamily(::std::uint32_t if_index) override;

 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

}  // namespace netconf
