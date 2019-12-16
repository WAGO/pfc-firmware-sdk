// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconfd {

class IIPManager {
 public:
  IIPManager() = default;
  virtual ~IIPManager() = default;

  IIPManager(const IIPManager&) = delete;
  IIPManager& operator=(const IIPManager&) = delete;
  IIPManager(const IIPManager&&) = delete;
  IIPManager& operator=(const IIPManager&&) = delete;

  virtual Status Configure(const IPConfigs& configs) const = 0;
  virtual bool IsApplicableToSystem(const IPConfigs& configs) const = 0;
  virtual Status ValidateIPConfigs(const IPConfigs& configs,
                                   const bool interference_has_to_be_checked) const = 0;
  virtual Status ValidateIPConfigIsApplicableToSystem(const IPConfigs& configs) const = 0;
  virtual IPConfigs GetIPConfigs(const Bridges& bridges) const = 0;

};

} /* namespace netconfd */
