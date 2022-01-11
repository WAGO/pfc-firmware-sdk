// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfigurator.hpp"

#include <numeric>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <numeric>
#include <boost/filesystem.hpp>
#include "TypesHelper.hpp"
#include "Logger.hpp"
#include "IPLink.hpp"

using namespace std::literals;

namespace netconf {

IPConfigurator::IPConfigurator(IIPController &ip_controller, const IDHCPClientController &dhcp_client_controller,
                               const IBootpClientController &bootp_client_controller, IIPLinks &ip_links)
    : ip_controller_ { ip_controller },
      dhcp_client_controller_ { dhcp_client_controller },
      bootp_client_controller_ { bootp_client_controller },
      ip_links_ { ip_links } {
}

void IPConfigurator::DeleteTempFiles(const Bridge &bridge) const {
  const boost::filesystem::path file = TMP_FILE_PATH + bridge;
  if (boost::filesystem::exists(file)) {
    boost::filesystem::remove(file);
  }

}

Status IPConfigurator::SetStatic(const IPConfig &ip_config) const {
  Status status;

  dhcp_client_controller_.StopClient(ip_config.interface_);
  bootp_client_controller_.StopClient(ip_config.interface_);
  DeleteTempFiles(ip_config.interface_);
  status = ip_controller_.SetIPConfig(ip_config);

  return status;
}

Status IPConfigurator::SetTemporary(const IPConfig &ip_config) const {

  return ip_controller_.SetIPConfig(ip_config);
}

Status IPConfigurator::SetDHCP(const IPConfig &ip_config) const {
  Status status;

  bootp_client_controller_.StopClient(ip_config.interface_);
  if (dhcp_client_controller_.GetStatus(ip_config.interface_) != DHCPClientStatus::RUNNING) {
    FlushIP(ip_config.interface_);
    DeleteTempFiles(ip_config.interface_);
    status = dhcp_client_controller_.StartClient(ip_config.interface_);
  }

  return status;
}

Status IPConfigurator::SetBootp(const IPConfig &ip_config) const {
  Status status;

  dhcp_client_controller_.StopClient(ip_config.interface_);
  FlushIP(ip_config.interface_);

  if (bootp_client_controller_.GetStatus(ip_config.interface_) != BootpClientStatus::RUNNING) {
    DeleteTempFiles(ip_config.interface_);
    status = bootp_client_controller_.StartClient(ip_config.interface_);
  }

  return status;
}

void IPConfigurator::SetNone(const IPConfig &ip_config) const {
  dhcp_client_controller_.StopClient(ip_config.interface_);
  bootp_client_controller_.StopClient(ip_config.interface_);
  FlushIP(ip_config.interface_);

  DeleteTempFiles(ip_config.interface_);
}

Status IPConfigurator::FlushIP(const Interface &interface) const {
  LogDebug("Flush IP of "s.append(interface));
  IPConfig config(interface, IPSource::TEMPORARY, ZeroIP, ZeroIP);
  return ip_controller_.SetIPConfig(config);
}

Status IPConfigurator::EnableGratuitousArp(const IPConfig &ip_config) const {

  //(arp_notify = 1) generates gratuitous arp requests when device is brought up or hardware address changes.
  // It does not if ip address changes!

  auto buffer_len = std::snprintf(nullptr, 0, "/proc/sys/net/ipv4/conf/%s/arp_notify", ip_config.interface_.c_str());

  assert(buffer_len > 0);  //NOLINT: do not implicitly decay an array into a pointer

  auto len_with_zero = static_cast<std::size_t>(buffer_len + 1);

  auto file_path = std::unique_ptr<char[]> { new char[len_with_zero] };

  std::snprintf(file_path.get(), len_with_zero, "/proc/sys/net/ipv4/conf/%s/arp_notify", ip_config.interface_.c_str());

  auto arp_notify = std::ofstream { file_path.get() };

  if (arp_notify.good()) {
    arp_notify << '1';
    arp_notify.flush();
    arp_notify.close();
  }

  return arp_notify.good() ? Status { StatusCode::OK } : Status { StatusCode::GENERIC_ERROR,
                                 "Failed to enable gratuitous arp" };
}

Status IPConfigurator::Configure(const netconf::IPConfig &ip_config) const {

  Status set_status = EnableGratuitousArp(ip_config);
  switch (ip_config.source_) {
    case IPSource::STATIC:
      set_status = SetStatic(ip_config);
      break;
    case IPSource::DHCP:
      set_status = SetDHCP(ip_config);
      break;
    case IPSource::BOOTP:
      set_status = SetBootp(ip_config);
      break;
    case IPSource::TEMPORARY:
      set_status = SetTemporary(ip_config);
      break;
    case IPSource::NONE:
      SetNone(ip_config);
      break;
    case IPSource::FIXIP:
      SetStatic(ip_config);
      break;
    case IPSource::EXTERNAL:
      SetNone(ip_config);
      break;
    default:
      set_status = Status { StatusCode::IP_CONFIG_SOURCE, ip_config.interface_ };
      break;
  }

  return set_status;
}

} /* namespace netconf */
