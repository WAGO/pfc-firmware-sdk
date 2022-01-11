// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

enum class BootpClientStatus {
  RUNNING,
  STOPPED,
};

class IBootpClientController {
 public:
  IBootpClientController() = default;
  virtual ~IBootpClientController() = default;

  IBootpClientController(const IBootpClientController&) = delete;
  IBootpClientController& operator=(const IBootpClientController&) = delete;
  IBootpClientController(IBootpClientController&&) = delete;
  IBootpClientController& operator=(IBootpClientController&&) = delete;

  virtual netconf::Status StartClient(const netconf::Bridge& bridge) const = 0;
  virtual void StopClient(const netconf::Bridge& bridge) const = 0;
  virtual BootpClientStatus GetStatus(const netconf::Bridge& bridge) const = 0;
};

} /* namespace netconf */
