// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "IBootpClientController.hpp"
#include "Types.hpp"
#include "IIPController.hpp"
#include "IDHCPClientController.hpp"
#include "IIPLinks.hpp"
#include "IIPConfigure.hpp"

namespace netconf {

class IPConfigurator : public IIPConfigure {
 public:
  IPConfigurator(IIPController& ip_controller,
                 const IDHCPClientController& dhcp_client_controller,
                 const IBootpClientController& bootp_client_controller,
                 IIPLinks& ip_links);
  virtual ~IPConfigurator() = default;

  IPConfigurator(const IPConfigurator&) = delete;
  IPConfigurator& operator=(const IPConfigurator&) = delete;
  IPConfigurator(const IPConfigurator&&) = delete;
  IPConfigurator& operator=(const IPConfigurator&&) = delete;

  Status Configure(const IPConfig& config) const override;

 private:

  Status SetStatic(const IPConfig& ip_config) const;
  Status SetTemporary(const IPConfig& ip_config) const;
  Status SetDHCP(const IPConfig& ip_config) const;
  Status SetBootp(const IPConfig& ip_config) const;
  void SetNone(const IPConfig& ip_config) const;

  Status EnableGratuitousArp(const IPConfig& ip_config) const;

  Status FlushIP(const Interface& interface) const;

  void DeleteTempFiles(const Bridge& bridge) const;


  IIPController& ip_controller_;
  const IDHCPClientController& dhcp_client_controller_;
  const IBootpClientController& bootp_client_controller_;
  IIPLinks& ip_links_;

  const ::std::string TMP_FILE_PATH = "/tmp/dhcp-bootp-data-";
};

} /* namespace netconf */
