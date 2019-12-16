// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDHCPClientController.hpp"
#include "ICommandExecutor.hpp"
#include "IDevicePropertiesProvider.hpp"
#include "IFileEditor.hpp"

namespace netconfd {

class DHCPClientController : public IDHCPClientController{
 public:

  DHCPClientController(const ICommandExecutor& command_executor,
                       const IDevicePropertiesProvider& properties_provider,
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
  const ICommandExecutor& command_executor_;
  const IDevicePropertiesProvider& properties_provider_;
  const IFileEditor& file_editor_;

  const ::std::string DHCP_CLIENT_PATH = "/sbin/udhcpc";
};

} /* namespace netconfd */
