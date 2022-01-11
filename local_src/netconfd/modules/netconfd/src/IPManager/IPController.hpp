// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IIPController.hpp"

namespace netconf {

class IPController : public IIPController {
 public:
  IPController() = default;
  virtual ~IPController() = default;

  IPController(const IPController&) = delete;
  IPController& operator=(const IPController&) = delete;
  IPController(IPController&&) = delete;
  IPController& operator=(IPController&&) = delete;

  Status SetIPConfig(const IPConfig& config) const override;

};

} /* namespace netconf */
