// SPDX-License-Identifier: LGPL-3.0-or-later

#include "NetlinkAddressCache.hpp"

#include <netlink/cache.h>
#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/types.h>

#include <boost/format.hpp>
#include "NetworkHelper.hpp"
#include <exception>
#include <system_error>

#include "Logger.hpp"
#include "Types.hpp"

namespace netconf {

static IIPEvent::ChangeType ConvertNetlinkAction(int netlink_act) {
  switch (netlink_act) {
    case NL_ACT_NEW:
      return IIPEvent::ChangeType::Add;
    case NL_ACT_DEL:
      return IIPEvent::ChangeType::Delete;
    case NL_ACT_CHANGE:
      return IIPEvent::ChangeType::Modify;
    default:
      // TODO(Pnd) LogWarning("ConvertNetlinkAction unhandled netlink action: " + ::std::to_string(netlink_act));
      return IIPEvent::ChangeType::Modify;
  }
}

class NetlinkAddressCache::Impl {
 public:
  Impl() = delete;
  Impl(nl_sock* nl_sock, nl_cache_mngr* nl_cache_mgr) : event_handler_{nullptr}, nl_cache_{nullptr} {
    if (rtnl_addr_alloc_cache(nl_sock, &nl_cache_) < 0) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "Error allocating rtnl cache");
    }

    if (nl_cache_mngr_add_cache(nl_cache_mgr, nl_cache_, CacheChange, this) < 0) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "Error adding cache to manager");
    }
  }

  static void CacheChange(nl_cache* cache, nl_object* obj, int change_type, void* user) {
    (void)cache;

    try {
      auto* address_cache_impl = reinterpret_cast<Impl*>(user);  // NOLINT: Need reinterpret_cast to cast from void*.

      auto rt_nl_addr =
          reinterpret_cast<rtnl_addr*>(nl_object_priv(obj));  // NOLINT: Need reinterpret_cast to cast from void*.

      address_cache_impl->CallEventHandler(rt_nl_addr, change_type);
    } catch (::std::exception& e) {
     // LogError(e.what());
    }
  }

  void CallEventHandler(rtnl_addr* rtnl_addr, int change_type) {
    if (event_handler_ == nullptr) return; // NOLINT: oneline statement, readability

    int if_index = rtnl_addr_get_ifindex(rtnl_addr);

    auto address = GetIPAddress(rtnl_addr);
    auto netmask = GetNetmask(rtnl_addr);

    event_handler_->OnAddressChange(ConvertNetlinkAction(change_type), static_cast<uint32_t>(if_index), address,
                                    netmask);
  }

  rtnl_addr* GetRtnlAddress(int if_index) {
    struct param {
      int if_index;
      struct rtnl_addr* addr;
    } param { if_index, nullptr};

    auto cb = [] (nl_object* o, void* p) {
      auto addr = reinterpret_cast<struct rtnl_addr*>(o); // NOLINT: Need reinterpret_cast to cast from nl_object*.
      auto lp = reinterpret_cast<struct param*>(p); // NOLINT: Need reinterpret_cast to cast from void*.
      if (addr == nullptr){ return; }
      if (rtnl_addr_get_ifindex(addr) != lp->if_index){ return; }

      lp->addr = addr;
    };

    nl_cache_foreach(nl_cache_, cb, &param);

    return param.addr;
  }

  ::std::string GetNetmask(rtnl_addr* rtnl_addr) {
    int prefix_len = rtnl_addr_get_prefixlen(rtnl_addr);
    return netconf::GetNetmask(static_cast<uint32_t>(prefix_len));
  }

  ::std::string GetNetmask(::std::uint32_t if_index) {
    auto rtnl_addr = GetRtnlAddress(static_cast<int>(if_index));
    return rtnl_addr == nullptr ? static_cast<const char*>(ZeroIP) : GetNetmask(rtnl_addr);
  }

  ::std::string GetIPAddress(rtnl_addr* rtnl_addr) {
    auto local = rtnl_addr_get_local(rtnl_addr);

    char address_buffer[INET_ADDRSTRLEN] = {0};
    nl_addr2str(local, static_cast<char*>(address_buffer), sizeof(address_buffer));
    ::std::string address(static_cast<char*>(address_buffer));

    std::size_t pos = address.find('/');
    if (pos != ::std::string::npos) {
      address.erase(address.begin() + static_cast<int>(pos), address.end());
    }

    return address;
  }

  ::std::string GetIPAddress(::std::uint32_t if_index) {
    auto rtnl_addr = GetRtnlAddress(static_cast<int>(if_index));
    return rtnl_addr == nullptr ? std::string{ZeroIP} : GetIPAddress(rtnl_addr);
  }

  IIPEvent* event_handler_;
  struct nl_cache* nl_cache_;
};  // namespace netconf

NetlinkAddressCache::NetlinkAddressCache(struct nl_sock* nl_sock, struct nl_cache_mngr* nl_cache_mgr) {
  impl_ = ::std::make_unique<NetlinkAddressCache::Impl>(nl_sock, nl_cache_mgr);
}

NetlinkAddressCache::~NetlinkAddressCache() = default;

void NetlinkAddressCache::Resync(struct nl_sock* nl_sock) {
  auto resync_result = nl_cache_resync(nl_sock, impl_->nl_cache_, &NetlinkAddressCache::Impl::CacheChange, impl_.get());
  if (resync_result < 0) {
    auto message = (boost::format("NetlinkDataReady: resync error #%1%") % resync_result).str();
    //TODO(Pnd) LogError(message);
  }
}

void NetlinkAddressCache::RegisterEventHandler(IIPEvent& event_handler) {
  impl_->event_handler_ = &event_handler;
}

void NetlinkAddressCache::UnregisterEventHandler(IIPEvent& event_handler) {
  if (impl_->event_handler_ == &event_handler) {
    impl_->event_handler_ = nullptr;
  }
}

::std::string NetlinkAddressCache::GetIPAddress(::std::uint32_t if_index) {
  return impl_->GetIPAddress(if_index);
}

::std::string NetlinkAddressCache::GetNetmask(::std::uint32_t if_index) {
  return impl_->GetNetmask(if_index);
}

}  // namespace netconf
