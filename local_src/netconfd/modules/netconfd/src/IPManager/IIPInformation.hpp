// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IIPInformation {
 public:
  IIPInformation() = default;
  virtual ~IIPInformation() = default;

  IIPInformation(const IIPInformation&) = delete;
  IIPInformation& operator=(const IIPInformation&) = delete;
  IIPInformation(IIPInformation&&) = delete;
  IIPInformation& operator=(IIPInformation&&) = delete;

  virtual IPConfigs GetCurrentIPConfigs() const = 0;
};

}
