// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

enum class DHCPClientStatus {
  RUNNING,
  STOPPED,
};

class IDHCPClientController {
 public:
  IDHCPClientController() = default;
  virtual ~IDHCPClientController() = default;

  IDHCPClientController(const IDHCPClientController&) = delete;
  IDHCPClientController& operator=(const IDHCPClientController&) = delete;
  IDHCPClientController(const IDHCPClientController&&) = delete;
  IDHCPClientController& operator=(const IDHCPClientController&&) = delete;

  virtual Status StartClient(const Bridge& bridge) const = 0;
  virtual Status StopClient(const Bridge& bridge) const = 0;
  virtual DHCPClientStatus GetStatus(const Bridge& bridge) const = 0;
};

} /* namespace netconf */
