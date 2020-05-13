// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

namespace netconfd {

class IIPConfiguration {
 public:

  virtual Status Configure(IPConfigs const& config) const = 0;

 private:
};


}  // namespace netconfd

