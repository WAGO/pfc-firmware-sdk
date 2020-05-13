// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "INetlinkMonitor.hpp"
#include <memory>

namespace netconfd
{

  class NetlinkMonitor :public INetlinkMonitor
  {
   public:

    NetlinkMonitor();
    virtual ~NetlinkMonitor();
    NetlinkMonitor(const NetlinkMonitor&) = delete;
    NetlinkMonitor(NetlinkMonitor&&) = delete;

  protected:
    nl_sock* GetNlSocket() override;
    nl_cache_mngr* GetNlCacheMngr() override;
    void AddCache(::std::shared_ptr<NetlinkCache>) override;

   private:
    class Impl;
    ::std::unique_ptr<Impl> pimpl_;

  };


}
