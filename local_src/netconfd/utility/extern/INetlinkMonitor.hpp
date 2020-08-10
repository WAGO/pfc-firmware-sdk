// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <cstdint>
#include <memory>
#include "NetlinkCache.hpp"

namespace netconf {

class INetlinkMonitor {
 public:
  virtual ~INetlinkMonitor() = default;

  template<class TCache, typename... Args>
  ::std::shared_ptr<TCache> Add(Args&&... args) {
    auto cache = ::std::make_shared<TCache>(GetNlSocket(), GetNlCacheMngr(),::std::forward<Args>(args)...);
    AddCache(static_cast<::std::shared_ptr<NetlinkCache>>(cache));
    return cache;
  }

  protected:
    virtual nl_sock* GetNlSocket() = 0;
    virtual nl_cache_mngr* GetNlCacheMngr() = 0;
    virtual void AddCache(::std::shared_ptr<NetlinkCache>) = 0;

};


}
