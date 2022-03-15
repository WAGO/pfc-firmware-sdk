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

  Status Configure(const Interface &itf_name, const Address &address, const Netmask &netmask) const override;
  Status Configure(const IPConfig &config) const override;
  void Flush(const Interface &itf_name) const override;

 private:
  Status SetIPConfig(const Interface &itf_name, const Address &address, const Netmask &netmask) const;

};

} /* namespace netconf */
