// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"
#include "NetworkInterfaceConstants.hpp"

namespace netconf
{

  class INetworkDevice
  {
    public:
      INetworkDevice() = default;
      virtual ~INetworkDevice() = default;

      INetworkDevice(const INetworkDevice&) = default;
      INetworkDevice& operator=(const INetworkDevice&) = default;
      INetworkDevice(INetworkDevice&&) = default;
      INetworkDevice& operator=(INetworkDevice&&) = default;

      virtual int GetIfIndex() const = 0;
      virtual Status SetState(DeviceState state) = 0;
      virtual DeviceState GetState() const = 0;


  };

} /* namespace netconf */
