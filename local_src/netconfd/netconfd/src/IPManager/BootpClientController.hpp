// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBootpClientController.hpp"

namespace netconfd {

class BootpClientController : public IBootpClientController {
 public:
  BootpClientController() = default;
  virtual ~BootpClientController() = default;

  BootpClientController(const BootpClientController&) = delete;
  BootpClientController& operator=(const BootpClientController&) = delete;
  BootpClientController(const BootpClientController&&) = delete;
  BootpClientController& operator=(const BootpClientController&&) = delete;

  Status StartClient(const Bridge& bridge) const override;
  Status StopClient(const Bridge& bridge) const override;
  BootpClientStatus GetStatus(const Bridge& bridge) const override;
  void DeleteTempFiles(const Bridge& bridge) const;

 private:
  const ::std::string BOOTP_SCRIPT_PATH = "/sbin/bootpc-startup";
};

} /* namespace netconfd */
