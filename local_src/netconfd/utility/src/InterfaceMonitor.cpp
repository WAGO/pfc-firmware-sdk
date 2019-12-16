// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     InterfaceMonitor.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <InterfaceMonitor.hpp>

#include <gio/gio.h>
#include <sys/types.h>
#include <system_error>
#include <atomic>
#include <mutex>
#include <sys/socket.h>
#include <linux/if_arp.h>
#include <netlink/netlink.h>
#include <netlink/types.h>
#include <netlink/route/link.h>
#include <netlink/cache.h>

#include <boost/format.hpp>
#include "Logger.hpp"

namespace netconfd {

using Lock = ::std::lock_guard<::std::mutex>;
using eth::InterfaceLinkState;
class InterfaceMonitor::Impl {

 public:
  Impl();
  ~Impl();

  Impl(const Impl&) = delete;
  Impl(Impl&&) = delete;
  Impl& operator= (const Impl&) = delete;
  Impl& operator= (Impl&&) = delete;


  void RegisterEventHandler(IInterfaceEvent& event_handler);
  void UnregisterEventHandler(IInterfaceEvent& event_handler);

  static void CacheChange(struct nl_cache * cache, struct nl_object *obj, int num, void * user);
  void CallEventHandler(uint32_t if_index, InterfaceLinkState new_state);
  ::std::uint32_t GetIffFlags(::std::uint32_t if_index);
  ::std::int32_t GetAddressFamily(::std::uint32_t if_index);

 private:
  static gboolean NetlinkDataReady(GSocket *socket, GIOCondition condition, gpointer garg);

  bool active_;
  struct nl_sock *nl_sock_;
  struct nl_cache_mngr *nl_mngr_;
  struct nl_cache *nl_cache_;
  IInterfaceEvent* event_handler_;
  ::std::mutex event_handler_mutex_;
  ::std::unique_ptr<GSource, std::function<void(GSource*)> > nl_source_;
  ::std::unique_ptr<GSocket, std::function<void(gpointer)> > nl_gsocket_;
  guint nl_source_id_;
};

InterfaceMonitor::Impl::Impl()
    : nl_sock_{nl_socket_alloc()},
      nl_mngr_{nullptr},
      nl_cache_{nullptr},
      event_handler_{nullptr}
{

  if (nl_cache_mngr_alloc(nl_sock_, NETLINK_ROUTE, NL_AUTO_PROVIDE, &nl_mngr_) < 0) {
    throw ::std::system_error(EINVAL, ::std::system_category(), "Error allocating cache manager");
  }

  if (rtnl_link_alloc_cache(nl_sock_, AF_UNSPEC, &nl_cache_) < 0) {
    throw ::std::system_error(EINVAL, ::std::system_category(), "Error allocating rtnl cache");
  }

  if (nl_cache_mngr_add_cache(nl_mngr_, nl_cache_, CacheChange, this) < 0) {
    throw ::std::system_error(EINVAL, ::std::system_category(), "Error adding cache to manager");
  }
  active_ = true;

  GError *gerror;
  auto fd = nl_socket_get_fd(nl_sock_);

  auto gnl_socket = g_socket_new_from_fd(fd, &gerror);
  if (gnl_socket == nullptr) {
    throw ::std::system_error(EINVAL, ::std::system_category(), "gnl_socket is null\n");
  }

  nl_gsocket_ = ::std::unique_ptr<GSocket, std::function<void(gpointer)> > { gnl_socket, g_object_unref };
  auto nl_gsource = g_socket_create_source(gnl_socket, G_IO_IN, nullptr);

  if (nl_gsource == nullptr) {
    throw ::std::system_error(EINVAL, ::std::system_category(), "nl_gsource is null\n");
  }

  g_source_set_callback(nl_gsource, reinterpret_cast<GSourceFunc>(Impl::NetlinkDataReady), // NOLINT: Need reinterpret_cast to cast to gpointer.
                        static_cast<gpointer>(this), nullptr);
  nl_source_ = ::std::unique_ptr<GSource, std::function<void(GSource*)> > { nl_gsource, g_source_unref };
  nl_source_id_ = g_source_attach(nl_gsource, nullptr);
  if (nl_source_id_ == 0) {
    throw ::std::system_error(EINVAL, ::std::system_category(), "g_source_attach failed\n");
  }
}

InterfaceMonitor::Impl::~Impl() {
  active_ = false;

  g_source_remove(nl_source_id_);

  nl_close(nl_sock_);

  nl_cache_mngr_free(nl_mngr_);
  nl_socket_free(nl_sock_);

}

void InterfaceMonitor::Impl::CallEventHandler(uint32_t if_index, InterfaceLinkState new_state) {
  Lock lock(event_handler_mutex_);
  if (event_handler_ != nullptr) {
    event_handler_->LinkChange(if_index, new_state);
  }
}

void InterfaceMonitor::Impl::CacheChange(struct nl_cache * cache, struct nl_object *obj, int num, void * user) {
  (void) cache;
  (void) num;
  auto *if_monitor = reinterpret_cast<Impl *>(user); // NOLINT: Need reinterpret_cast to cast from void*.

  auto *l = reinterpret_cast<rtnl_link *>(nl_object_priv(obj)); // NOLINT: Need reinterpret_cast to cast from void*.

  uint32_t flags = rtnl_link_get_flags(l);
  auto if_index = static_cast<uint32_t>(rtnl_link_get_ifindex(l));

  InterfaceLinkState new_state = ((flags & IFF_LOWER_UP) == IFF_LOWER_UP)
      ? InterfaceLinkState::Up
      : InterfaceLinkState::Down;

  if_monitor->CallEventHandler(if_index, new_state);
}

::std::int32_t InterfaceMonitor::Impl::GetAddressFamily(::std::uint32_t if_index) {
  struct rtnl_link * link = rtnl_link_get(nl_cache_, static_cast<int32_t>(if_index));
  int32_t family = 0;
  if (link != nullptr) {
    family = rtnl_link_get_family(link);
  }
  rtnl_link_put(link);

  return family;
}

::std::uint32_t InterfaceMonitor::Impl::GetIffFlags(::std::uint32_t if_index) {
  struct rtnl_link * link = rtnl_link_get(nl_cache_, static_cast<std::int32_t>(if_index));
  uint32_t flags = 0;
  if (link != nullptr) {
    flags = rtnl_link_get_flags(link);
  }
  rtnl_link_put(link);

  return flags;
}

gboolean InterfaceMonitor::Impl::NetlinkDataReady(__attribute__((unused))GSocket *socket, __attribute__((unused))GIOCondition condition,
                                                gpointer garg) {
  auto *if_monitor = reinterpret_cast<InterfaceMonitor::Impl *>(garg); // NOLINT: Need reinterpret_cast to cast from gpointer.

  auto result = nl_cache_mngr_data_ready(if_monitor->nl_mngr_);
  if (result < 0) {
    auto message = (boost::format("NetlinkDataReady: error #%1%, manually resync cache") % result).str();
    LogDebug(message);

    auto resync_result = nl_cache_resync(if_monitor->nl_sock_, if_monitor->nl_cache_,
                                         &InterfaceMonitor::Impl::CacheChange, garg);
    if (resync_result < 0) {
      message = (boost::format("NetlinkDataReady: resync error #%1%") % resync_result).str();
      LogError(message);
    }

  }
  return if_monitor->active_ ? 1 : 0;
}

InterfaceMonitor::InterfaceMonitor() {
  pimpl_ = std::make_unique<Impl>();
}

InterfaceMonitor::~InterfaceMonitor(){ //NOLINT(modernize-use-equals-default) : Need this dtor for realizing PIMPL idiom with unique_ptr.

}

void InterfaceMonitor::RegisterEventHandler(IInterfaceEvent& event_handler) {
  pimpl_->RegisterEventHandler(event_handler);
}

void InterfaceMonitor::Impl::RegisterEventHandler(IInterfaceEvent& event_handler) {
  Lock lock(event_handler_mutex_);
  event_handler_ = &event_handler;
}

void InterfaceMonitor::UnregisterEventHandler(IInterfaceEvent& event_handler) {
  pimpl_->UnregisterEventHandler(event_handler);
}

void InterfaceMonitor::Impl::UnregisterEventHandler(IInterfaceEvent& event_handler) {
  Lock lock(event_handler_mutex_);
  if (&event_handler == event_handler_) {
    event_handler_ = nullptr;
  }
}

::std::uint32_t InterfaceMonitor::GetIffFlags(::std::uint32_t if_index) {
  return pimpl_->GetIffFlags(if_index);
}

::std::int32_t InterfaceMonitor::GetAddressFamily(::std::uint32_t if_index) {
  return pimpl_->GetAddressFamily(if_index);
}

}  // end namespace
//---- End of source file ------------------------------------------------------
