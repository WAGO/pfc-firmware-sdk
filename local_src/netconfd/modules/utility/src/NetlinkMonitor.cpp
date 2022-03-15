// SPDX-License-Identifier: LGPL-3.0-or-later
#include "NetlinkMonitor.hpp"

#include <gio/gio.h>
#include <netlink/cache.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/types.h>

#include <boost/format.hpp>
#include <system_error>
#include <vector>

#include "Logger.hpp"

namespace netconf {

class NetlinkMonitor::Impl {
 public:
  Impl() : nl_sock_{nl_socket_alloc()}, nl_mngr_{nullptr} {
    if (nl_cache_mngr_alloc(nl_sock_, NETLINK_ROUTE, NL_AUTO_PROVIDE, &nl_mngr_) < 0) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "Error allocating cache manager");
    }

    active_ = true;

    GError *gerror;
    auto fd = nl_socket_get_fd(nl_sock_);

    auto gnl_socket = g_socket_new_from_fd(fd, &gerror);
    if (gnl_socket == nullptr) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "gnl_socket is null\n");
    }

    nl_gsocket_     = ::std::unique_ptr<GSocket, std::function<void(gpointer)>>{gnl_socket, g_object_unref};
    auto nl_gsource = g_socket_create_source(gnl_socket, G_IO_IN, nullptr);

    if (nl_gsource == nullptr) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "nl_gsource is null\n");
    }

    g_source_set_callback(
        nl_gsource,
        reinterpret_cast<GSourceFunc>(Impl::NetlinkDataReady),  // NOLINT: Need reinterpret_cast to cast to gpointer.
        static_cast<gpointer>(this), nullptr);

    nl_source_    = ::std::unique_ptr<GSource, std::function<void(GSource *)>>{nl_gsource, g_source_unref};
    nl_source_id_ = g_source_attach(nl_gsource, nullptr);
    if (nl_source_id_ == 0) {
      throw ::std::system_error(EINVAL, ::std::system_category(), "g_source_attach failed\n");
    }
  }

  ~Impl() {
    active_ = false;

    g_source_remove(nl_source_id_);

    nl_close(nl_sock_);

    nl_cache_mngr_free(nl_mngr_);
    nl_socket_free(nl_sock_);
  }

  Impl(const Impl &) = delete;
  Impl(Impl &&)      = delete;
  Impl &operator=(const Impl &) = delete;
  Impl &operator=(Impl &&) = delete;

  void AddCache(::std::shared_ptr<NetlinkCache> &cache) {
    caches_.push_back(cache);
  }

  nl_sock *GetNlSocket() {
    return nl_sock_;
  }

  nl_cache_mngr *GetNlCacheMngr() {
    return nl_mngr_;
  }

 private:
  static gboolean NetlinkDataReady(__attribute__((unused)) GSocket *socket,
                                   __attribute__((unused)) GIOCondition condition, gpointer garg) {
    auto *monitor =
        reinterpret_cast<NetlinkMonitor::Impl *>(garg);  // NOLINT: Need reinterpret_cast to cast from gpointer.

    auto result = nl_cache_mngr_data_ready(monitor->nl_mngr_);
    if (result < 0) {
      auto message = (boost::format("NetlinkDataReady: error #%1%, manually resync cache") % result).str();

      for (auto &cache : monitor->caches_) {
        cache->Resync(monitor->nl_sock_);
      }
    }
    return monitor->active_ ? 1 : 0;
  }

  bool active_;
  nl_sock *nl_sock_;
  nl_cache_mngr *nl_mngr_;
  ::std::unique_ptr<GSource, std::function<void(GSource *)>> nl_source_;
  ::std::unique_ptr<GSocket, std::function<void(gpointer)>> nl_gsocket_;
  guint nl_source_id_;
  ::std::vector<::std::shared_ptr<NetlinkCache>> caches_;
};

nl_sock *NetlinkMonitor::GetNlSocket() {
  return pimpl_->GetNlSocket();
}

nl_cache_mngr *NetlinkMonitor::GetNlCacheMngr() {
  return pimpl_->GetNlCacheMngr();
}

NetlinkMonitor::NetlinkMonitor() {
  pimpl_ = std::make_unique<Impl>();
}

void NetlinkMonitor::AddCache(::std::shared_ptr<NetlinkCache> cache) {
  pimpl_->AddCache(cache);
}

NetlinkMonitor::~NetlinkMonitor() = default;

}  // namespace netconf
