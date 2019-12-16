// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfigurator.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "Logger.hpp"

using namespace std::literals;

namespace netconfd {

IPConfigurator::IPConfigurator(
    IIPController& ip_controller,
    const IDHCPClientController& dhcp_client_controller,
    const IBootpClientController& bootp_client_controller,
    const IInterfaceInformation& itf_info)
    : ip_controller_ { ip_controller },
      dhcp_client_controller_ { dhcp_client_controller },
      bootp_client_controller_ { bootp_client_controller },
      itf_info_ { itf_info } {
}

void IPConfigurator::DeleteTempFiles(const Bridge& bridge) const {
  const boost::filesystem::path file = TMP_FILE_PATH + bridge;
  if (boost::filesystem::exists(file)) {
    boost::filesystem::remove(file);
  }
}

IPConfigs IPConfigurator::GetConfigurations(const Bridges &bridges) const {

  IPConfigs configs;

  for (const auto &bridge : bridges) {
    IPConfig ip_config;
    Status status = ip_controller_.GetIPConfig(bridge, ip_config);
    if (status.NotOk()) {
      configs.clear();
      break;
    }

    auto bootp_status = bootp_client_controller_.GetStatus(bridge);
    DHCPClientStatus dhcp_status = dhcp_client_controller_.GetStatus(bridge);

    if (DHCPClientStatus::RUNNING == dhcp_status) {
      ip_config.source_ = IPSource::DHCP;
    } else if (bootp_status == BootpClientStatus::RUNNING) {
      ip_config.source_ = IPSource::BOOTP;
    } else {
      ip_config.source_ = IPSource::STATIC;
    }

    configs.push_back(ip_config);
  }

  return configs;
}

Status IPConfigurator::SetStatic(const IPConfig &ip_config) const {
  Status status;
  dhcp_client_controller_.StopClient(ip_config.interface_);
  bootp_client_controller_.StopClient(ip_config.interface_);
  DeleteTempFiles(ip_config.interface_);

  bool is_up = false;

  if (itf_info_.IsBridge(ip_config.interface_)) {
    Bridge bridge { ip_config.interface_ };
    is_up = itf_info_.HasAnyInterfacesUp(bridge);
  } else {
    is_up = itf_info_.IsInterfaceUp(ip_config.interface_);
  }

  if (is_up) {
    status = ip_controller_.SetIPConfig(ip_config);
  } else {
    status = FlushIP(ip_config.interface_);
  }

  return status;
}

Status IPConfigurator::SetTemporary(const IPConfig &ip_config) const {

  return ip_controller_.SetIPConfig(ip_config);
}

Status IPConfigurator::SetDHCP(const IPConfig &ip_config) const {

  Status status;
  bool isUp = false;

  if (itf_info_.IsBridge(ip_config.interface_)) {
    Bridge b { ip_config.interface_ };
    isUp = itf_info_.HasAnyInterfacesUp(b);
  } else {
    isUp = itf_info_.IsInterfaceUp(ip_config.interface_);
  }

  bootp_client_controller_.StopClient(ip_config.interface_);

  if (isUp) {
    if (dhcp_client_controller_.GetStatus(ip_config.interface_)
        != DHCPClientStatus::RUNNING) {
      FlushIP(ip_config.interface_);
      DeleteTempFiles(ip_config.interface_);
      status = dhcp_client_controller_.StartClient(ip_config.interface_);
    }
  } else {
    dhcp_client_controller_.StopClient(ip_config.interface_);
    DeleteTempFiles(ip_config.interface_);
    FlushIP(ip_config.interface_);
    LogDebug(
        "No interface of bridge "s.append(ip_config.interface_).append(
            " is up. Do not start DHCP client service."));
  }

  return status;
}

Status IPConfigurator::FlushIP(const Interface& interface) const {
  Status status;
  LogDebug("Flush IP of "s.append(interface));
  IPConfig config(interface, IPSource::TEMPORARY, "0.0.0.0", "0.0.0.0");
  status = ip_controller_.SetIPConfig(config);
  if (status.NotOk()) {
    status.Prepend("Failed to flush IP");
  }

  return status;
}

Status IPConfigurator::SetBootp(const IPConfig &ip_config) const {

  Status status = dhcp_client_controller_.StopClient(ip_config.interface_);

  bool isUp = false;

  if (itf_info_.IsBridge(ip_config.interface_)) {
    Bridge b { ip_config.interface_ };
    isUp = itf_info_.HasAnyInterfacesUp(b);
  } else {
    isUp = itf_info_.IsInterfaceUp(ip_config.interface_);
  }

  if (isUp) {
    if (status.Ok()) {
      status = FlushIP(ip_config.interface_);
    }

    if (status.Ok()) {
      if (bootp_client_controller_.GetStatus(ip_config.interface_)
          != BootpClientStatus::RUNNING) {
        DeleteTempFiles(ip_config.interface_);
        status = bootp_client_controller_.StartClient(ip_config.interface_);
      }
    }
  } else {
    bootp_client_controller_.StopClient(ip_config.interface_);
    DeleteTempFiles(ip_config.interface_);
    FlushIP(ip_config.interface_);
    LogDebug(
        "No interface of bridge "s.append(ip_config.interface_).append(
            " is up. Do not start bootp client service."));
  }

  return status;
}

Status IPConfigurator::SetNone(const IPConfig &ip_config) const {

  Status status = dhcp_client_controller_.StopClient(ip_config.interface_);

  if (status.Ok()) {
    status = bootp_client_controller_.StopClient(ip_config.interface_);
  }

  if (status.Ok()) {
    status = FlushIP(ip_config.interface_);
  }

  DeleteTempFiles(ip_config.interface_);

  return status;
}

Status IPConfigurator::EnableGratuitousArp(const IPConfig &ip_config) const {
  auto buffer_len = std::snprintf(nullptr, 0,
                                  "/proc/sys/net/ipv4/conf/%s/arp_notify",
                                  ip_config.interface_.c_str());

  assert(buffer_len > 0);  //NOLINT: do not implicitly decay an array into a pointer

  auto len_with_zero = static_cast<std::size_t>(buffer_len + 1);

  auto file_path = std::unique_ptr<char[]> { new char[len_with_zero] };

  std::snprintf(file_path.get(), len_with_zero,
                "/proc/sys/net/ipv4/conf/%s/arp_notify",
                ip_config.interface_.c_str());

  auto arp_notify = std::ofstream { file_path.get() };

  if (arp_notify.good()) {
    arp_notify << '1';
    arp_notify.flush();
    arp_notify.close();
  }

  return arp_notify.good() ? Status { StatusCode::OK } : Status {
                                 StatusCode::FILE_WRITE_ERROR,
                                 "Failed to enable gratuitous arp" };
}

Status IPConfigurator::Configure(IPConfigs const &configs) const {
  Status status(StatusCode::OK);

  for (auto &ip_config : configs) {
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
        set_status = SetNone(ip_config);
        break;
      case IPSource::EXTERNAL:
        /* Do nothing here*/
        break;
      default:
        set_status = Status { StatusCode::INVALID_CONFIG, "Invalid IPSource" };
        break;
    }

    if (set_status.NotOk()) {
      status.Prepend(StatusCode::ERROR, set_status.GetMessage());
    }
  }

  return status;
}

} /* namespace netconfd */
