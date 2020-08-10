// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IBridgeInformation {
 public:
  IBridgeInformation() = default;
  virtual ~IBridgeInformation() = default;

  IBridgeInformation(const IBridgeInformation&) = delete;
  IBridgeInformation& operator=(const IBridgeInformation&) = delete;
  IBridgeInformation(const IBridgeInformation&&) = delete;
  IBridgeInformation& operator=(const IBridgeInformation&&) = delete;

  virtual Interfaces GetBridgeAssignedInterfaces() const = 0;
  virtual BridgeConfig GetBridgeConfig() const = 0;
};

}
