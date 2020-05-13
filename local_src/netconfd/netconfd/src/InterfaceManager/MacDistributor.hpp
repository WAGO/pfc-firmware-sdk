// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceProperties.hpp"
#include "Status.hpp"
#include "Types.hpp"
#include "IMacDistributor.hpp"

namespace netconfd {

class MacDistributor : public IMacDistributor{
 public:
  MacDistributor(IDeviceProperties& properties_provider);
  virtual ~MacDistributor() = default;

  MacDistributor(const MacDistributor&) = delete;
  MacDistributor& operator=(const MacDistributor&) = delete;
  MacDistributor(const MacDistributor&&) = delete;
  MacDistributor& operator=(const MacDistributor&&) = delete;

  Status SetMac(Bridge const& interface) override;

 private:
  Status SetMac(::std::string const& mac, ::std::string const& interface);

  IDeviceProperties& properties_provider_;
};

} /* namespace netconfd */
