#include "BridgeUtil.hpp"

#include <netlink/cache.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/route/link/bridge.h>
#include <netlink/socket.h>
#include <netlink/types.h>

#include <functional>

#include "Logger.hpp"
namespace netconf {

namespace {

using nl_sock_ptr = ::std::unique_ptr<nl_sock, ::std::function<void(nl_sock*)>>;
using nl_cache_ptr = ::std::unique_ptr<nl_cache, ::std::function<void(nl_cache*)>>;
using rtnl_link_ptr = ::std::unique_ptr<rtnl_link, ::std::function<void(rtnl_link*)>>;

nl_sock_ptr AllocateNetlinkSocket() {

  auto nl_socket_deleter = [](nl_sock *s) {
    if (s != nullptr) {
      nl_close(s);
      nl_socket_free(s);
    }
  };

  nl_sock_ptr socket { nl_socket_alloc(), nl_socket_deleter };
  nl_connect(socket.get(), NETLINK_ROUTE);
  return socket;
}



nl_cache_ptr AllocLinkCache(nl_sock& socket, int protocol) {
  auto nl_cache_deleter = [](nl_cache *c) {
    if (c != nullptr) {
      nl_cache_free(c);
    }
  };

  nl_cache *cache = nullptr;
  auto res = rtnl_link_alloc_cache(&socket, protocol, &cache);
  if (res != 0){
    LogError(::std::string("rtnl_link_alloc_cache: ") + nl_geterror(res));
  }

  return nl_cache_ptr { cache, nl_cache_deleter };
}

rtnl_link_ptr GetLink(nl_cache& cache, ::std::uint32_t if_index) {

  auto rtnl_link_put_deleter = [](rtnl_link *l) {
    if (l != nullptr) {
      rtnl_link_put(l);
    }
  };

  return rtnl_link_ptr { rtnl_link_get(&cache, static_cast<::std::int32_t>(if_index)), rtnl_link_put_deleter };
}

int NetlinkSetBridgeLinkLearning(::std::uint32_t if_index, bool learning) {
  auto socket = AllocateNetlinkSocket();
  auto cache = AllocLinkCache(*socket, AF_BRIDGE);
  auto link = GetLink(*cache, if_index);

  if (not link) {
    return -NLE_INVAL;
  }

  auto pl = link.get();
  if (learning) {
    rtnl_link_bridge_set_flags(pl, RTNL_BRIDGE_LEARNING);
  } else {
    rtnl_link_bridge_unset_flags(pl, RTNL_BRIDGE_LEARNING);
  }

  rtnl_link_change(socket.get(), pl, pl, 0);

  return 0;
}

int NetlinkGetBridgeLinkLearning(::std::uint32_t if_index, bool &learning) {
  auto socket = AllocateNetlinkSocket();
  auto cache = AllocLinkCache(*socket, AF_BRIDGE);
  auto link = GetLink(*cache, if_index);

  if (not link) {
    return -NLE_INVAL;
  }

  auto pl = link.get();
  auto flags = rtnl_link_bridge_get_flags(pl);
  learning = (flags & RTNL_BRIDGE_LEARNING) != 0;

  return 0;
}

}

using namespace std::string_literals;

MacLearning GetMacLearning(::std::uint32_t if_index) {

  bool learning;
  auto nl_status = NetlinkGetBridgeLinkLearning(if_index,learning);
  if (nl_status != NLE_SUCCESS) {
    LogError("NetlinkGetBridgeLinkLearning: "s + nl_geterror(nl_status));
    return MacLearning::UNKNOWN;
  }

  return  learning ? MacLearning::ON : MacLearning::OFF;
}

void SetMacLearning(::std::uint32_t if_index, MacLearning learning) {
  if(learning != MacLearning::UNKNOWN){
    auto nl_status = NetlinkSetBridgeLinkLearning(if_index, learning == MacLearning::ON);
    if(nl_status != NLE_SUCCESS){
      LogError("NetlinkSetBridgeLinkLearning: "s + nl_geterror(nl_status));
    }
  }
}

}  // namespace netconf
