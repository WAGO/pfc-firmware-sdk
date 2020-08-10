// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "NetlinkCache.hpp"

#include <memory>

#include "IIPMonitor.hpp"

namespace netconf {

class NetlinkAddressCache : public NetlinkCache, public IIPMonitor {
 public:
  NetlinkAddressCache() = delete;
  NetlinkAddressCache(nl_sock* nl_sock, nl_cache_mngr* nl_cache_mgr);
  ~NetlinkAddressCache();

  void Resync(nl_sock*) override;

  ::std::string GetIPAddress(::std::uint32_t if_index) override;
  ::std::string GetNetmask(::std::uint32_t if_index) override;

  void RegisterEventHandler(IIPEvent& event_handler) override;
  void UnregisterEventHandler(IIPEvent& event_handler) override;

 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

}  // namespace netconf
