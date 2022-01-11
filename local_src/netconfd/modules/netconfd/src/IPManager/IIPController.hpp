// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IIPController {
 public:
  IIPController() = default;
  virtual ~IIPController() = default;

  IIPController(const IIPController&) = delete;
  IIPController& operator=(const IIPController&) = delete;
  IIPController(IIPController&&) = delete;
  IIPController& operator=(IIPController&&) = delete;

  virtual Status SetIPConfig(const IPConfig& config) const = 0;

};

} /* namespace netconf */
