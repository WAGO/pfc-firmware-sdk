// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceProperties.hpp"
#include "IDHCPClientController.hpp"
#include "IFileEditor.hpp"

namespace netconf {

class DHCPClientController : public IDHCPClientController{
 public:

  DHCPClientController(const IDeviceProperties& properties_provider,
                       const IFileEditor& file_editor);
  virtual ~DHCPClientController() = default;

  DHCPClientController(const DHCPClientController&) = delete;
  DHCPClientController& operator=(const DHCPClientController&) = delete;
  DHCPClientController(const DHCPClientController&&) = delete;
  DHCPClientController& operator=(const DHCPClientController&&) = delete;

  Status StartClient(const Bridge& bridge) const override;
  Status StopClient(const Bridge& bridge) const override;
  DHCPClientStatus GetStatus(const Bridge& bridge) const override;

 private:
  const IDeviceProperties& properties_provider_;
  const IFileEditor& file_editor_;

  const ::std::string DHCP_CLIENT_PATH = "/sbin/udhcpc";
};

} /* namespace netconf */
