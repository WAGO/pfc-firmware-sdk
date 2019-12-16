// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconfd {

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
  IBootpClientController(const IBootpClientController&&) = delete;
  IBootpClientController& operator=(const IBootpClientController&&) = delete;

  virtual Status StartClient(const Bridge& bridge) const = 0;
  virtual Status StopClient(const Bridge& bridge) const = 0;
  virtual BootpClientStatus GetStatus(const Bridge& bridge) const = 0;
};

} /* namespace netconfd */
