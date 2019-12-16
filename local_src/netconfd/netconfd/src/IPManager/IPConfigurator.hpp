// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBootpClientController.hpp"
#include "Status.hpp"
#include "Types.hpp"
#include "IIPController.hpp"
#include "IDHCPClientController.hpp"
#include "IInterfaceInformation.hpp"

namespace netconfd {

class IPConfigurator {
 public:
  IPConfigurator(IIPController& ip_controller,
                 const IDHCPClientController& dhcp_client_controller,
                 const IBootpClientController& bootp_client_controller,
                 const IInterfaceInformation& itf_info);
  virtual ~IPConfigurator() = default;

  IPConfigurator(const IPConfigurator&) = delete;
  IPConfigurator& operator=(const IPConfigurator&) = delete;
  IPConfigurator(const IPConfigurator&&) = delete;
  IPConfigurator& operator=(const IPConfigurator&&) = delete;

  IPConfigs GetConfigurations(const Bridges& bridges) const;
  Status Configure(IPConfigs const& config) const;



 private:

  Status SetStatic(const IPConfig& ip_config) const;
  Status SetTemporary(const IPConfig& ip_config) const;
  Status SetDHCP(const IPConfig& ip_config) const;
  Status SetBootp(const IPConfig& ip_config) const;
  Status SetNone(const IPConfig& ip_config) const;

  Status EnableGratuitousArp(const IPConfig& ip_config) const;

  Status FlushIP(const Interface& interface) const;

  void DeleteTempFiles(const Bridge& bridge) const;

  IIPController& ip_controller_;
  const IDHCPClientController& dhcp_client_controller_;
  const IBootpClientController& bootp_client_controller_;
  const IInterfaceInformation& itf_info_;

  const ::std::string TMP_FILE_PATH = "/tmp/dhcp-bootp-data-";
};

} /* namespace netconfd */
