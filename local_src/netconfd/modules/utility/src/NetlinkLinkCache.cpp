// SPDX-License-Identifier: LGPL-3.0-or-later

#include "NetlinkLinkCache.hpp"

#include <netlink/cache.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/types.h>

#include <boost/format.hpp>
#include <exception>
#include <system_error>

#include <net/if.h>

#include "Logger.hpp"

namespace netconf {

namespace nl {
auto nl_obj_put_deleter = [](nl_object *o) { //NOLINT(cert-err58-cpp)
  if (o != nullptr){
    nl_object_put(o);
  }
};

using nl_object_ptr = std::unique_ptr<struct nl_object, decltype(nl_obj_put_deleter) >;

inline nl_object_ptr GetFromCache(nl_cache *cache, nl_object *obj) {
  return nl_object_ptr { nl_cache_find(cache, obj), nl_obj_put_deleter };
}

inline uint32_t GetIfIndex(struct nl_object *obj) {
  auto l = reinterpret_cast<rtnl_link*>(nl_object_priv(obj));  // NOLINT: Need reinterpret_cast to cast from void*.
  return static_cast<uint32_t>(rtnl_link_get_ifindex(l));
}

inline bool IsMasterEvent(struct nl_object *obj) {
  auto l = reinterpret_cast<rtnl_link*>(nl_object_priv(obj));  // NOLINT: Need reinterpret_cast to cast from void*.
  if (rtnl_link_get_family(l) == AF_BRIDGE) {
    if (rtnl_link_get_master(l) != 0) {
      return true;
    }
  }
  return false;
}

inline uint32_t GetIfFlags(struct nl_object *obj) {
  auto l = reinterpret_cast<rtnl_link*>(nl_object_priv(obj));  // NOLINT: Need reinterpret_cast to cast from void*.
  return rtnl_link_get_flags(l);
}

}  // namespace nl

class NetlinkLinkCache::Impl {
 public:
  Impl(struct nl_sock *nl_sock, struct nl_cache_mngr *nl_cache_mgr){
    if (rtnl_link_alloc_cache(nl_sock, AF_UNSPEC, &nl_cache_) < 0) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "Error allocating rtnl cache");
    }

    if (nl_cache_mngr_add_cache(nl_cache_mgr, nl_cache_, CacheChange, this) < 0) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "Error adding cache to manager");
    }
  }
  virtual ~Impl() = default;

  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;
  Impl(const Impl&&) = delete;
  Impl& operator=(const Impl&&) = delete;

  static void CacheChange(struct nl_cache *cache, struct nl_object *old_obj, int action, void *user) {
    static_assert(NL_ACT_NEW == static_cast<int>(IInterfaceEvent::Action::NEW));

    auto cache_obj = nl::GetFromCache(cache, old_obj);
    auto obj = cache_obj ? cache_obj.get() : old_obj;

    /* Skip entry in context of bridge:
     * e.g.: bridge ethX12 ether 00:30:de:44:bd:99 master br0 <broadcast,multicast,up,running,lowerup> slave-of eth0
     * We only want the entries without the bridge context
     */
    if (nl::IsMasterEvent(obj)) {
      return;
    }

    auto if_index = nl::GetIfIndex(obj);
    auto if_flags = nl::GetIfFlags(obj);
    auto if_action = static_cast<IInterfaceEvent::Action>(action);

    auto this_ = reinterpret_cast<Impl*>(user);  // NOLINT
    this_->CallEventHandler(if_index, if_flags, if_action);

  }

  void CallEventHandler(uint32_t if_index, uint32_t flags, IInterfaceEvent::Action action) {
    if (event_handler_ != nullptr) {
      event_handler_->LinkChange(if_index, flags, action);
    }
  }

  IInterfaceEvent *event_handler_ = nullptr;
  struct nl_cache *nl_cache_ = nullptr;
};

NetlinkLinkCache::NetlinkLinkCache(struct nl_sock *nl_sock, struct nl_cache_mngr *nl_cache_mgr) {
  impl_ = ::std::make_unique<NetlinkLinkCache::Impl>(nl_sock, nl_cache_mgr);
}

NetlinkLinkCache::~NetlinkLinkCache() = default;

void NetlinkLinkCache::Resync(struct nl_sock *nl_sock) {
  auto resync_result = nl_cache_resync(nl_sock, impl_->nl_cache_, &NetlinkLinkCache::Impl::CacheChange, impl_.get());
  if (resync_result < 0) {
    auto message = (boost::format("NetlinkDataReady: resync error #%1%") % resync_result).str();
    LogError(message);
  }
}

::std::int32_t NetlinkLinkCache::GetAddressFamily(::std::uint32_t if_index) {
  struct rtnl_link *link = rtnl_link_get(impl_->nl_cache_, static_cast<int32_t>(if_index));
  int32_t family = 0;
  if (link != nullptr) {
    family = rtnl_link_get_family(link);
  }
  rtnl_link_put(link);

  return family;
}

::std::uint32_t NetlinkLinkCache::GetIffFlags(::std::uint32_t if_index) {
  struct rtnl_link *link = rtnl_link_get(impl_->nl_cache_, static_cast<std::int32_t>(if_index));
  uint32_t flags = 0;
  if (link != nullptr) {
    flags = rtnl_link_get_flags(link);
  }
  rtnl_link_put(link);

  return flags;
}

void NetlinkLinkCache::RegisterEventHandler(IInterfaceEvent &event_handler) {
  impl_->event_handler_ = &event_handler;
}
void NetlinkLinkCache::UnregisterEventHandler(IInterfaceEvent &event_handler) {
  if (impl_->event_handler_ == &event_handler){
    impl_->event_handler_ = nullptr;
  }
}

}  // namespace netconf

