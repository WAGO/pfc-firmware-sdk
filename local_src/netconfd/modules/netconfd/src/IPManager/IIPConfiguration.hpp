// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

namespace netconf {

class IIPConfiguration {
 public:
  IIPConfiguration(const IIPConfiguration&) = delete;
  IIPConfiguration& operator=(const IIPConfiguration&) = delete;
  IIPConfiguration(IIPConfiguration&&) = delete;
  IIPConfiguration& operator=(IIPConfiguration&&) = delete;

  virtual Status Configure(IPConfigs const& config) const = 0;

 private:
};


}  // namespace netconf

