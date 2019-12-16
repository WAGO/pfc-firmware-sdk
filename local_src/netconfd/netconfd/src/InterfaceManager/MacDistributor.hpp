// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"
#include "IDevicePropertiesProvider.hpp"
#include "IMacDistributor.hpp"

namespace netconfd {

class MacDistributor : public IMacDistributor{
 public:
  MacDistributor(IDevicePropertiesProvider& properties_provider);
  virtual ~MacDistributor() = default;

  MacDistributor(const MacDistributor&) = delete;
  MacDistributor& operator=(const MacDistributor&) = delete;
  MacDistributor(const MacDistributor&&) = delete;
  MacDistributor& operator=(const MacDistributor&&) = delete;

  Status SetMac(Bridge const& interface) override;

 private:
  Status SetMac(::std::string const& mac, ::std::string const& interface);

  IDevicePropertiesProvider& properties_provider_;
};

} /* namespace netconfd */
