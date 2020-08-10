// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "INetDevManager.hpp"
#include "INetDevConstruction.hpp"

namespace netconf {

class MockINetDevManager : public INetDevManager {
 public:

  MOCK_METHOD1(RegisterForNetDevConstructionEvents,void(INetDevConstruction& netdev_construction));
  MOCK_METHOD1(GetByName, ::std::shared_ptr<NetDev>(::std::string name) );
  MOCK_METHOD1(GetByLabel, ::std::shared_ptr<NetDev>(::std::string name) );
  MOCK_METHOD1(GetByIfIndex, ::std::shared_ptr<NetDev>(::std::uint32_t if_index) );
  MOCK_METHOD0(GetEthernetNetDevs, NetDevs () );
  MOCK_METHOD0(GetBridgeNetDevs, NetDevs () );
  MOCK_METHOD0(GetPortNetDevs, NetDevs () );
  MOCK_METHOD0(GetNetDevs, NetDevs () );
  MOCK_METHOD1(GetNetDevs, NetDevs (DeviceType) );
  MOCK_METHOD1(ConfigureBridges,void(const BridgeConfig&));
};


}  // namespace netconf
