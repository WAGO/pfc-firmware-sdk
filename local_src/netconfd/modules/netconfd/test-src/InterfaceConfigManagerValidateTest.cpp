// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <MockIPersistencePortConfigs.hpp>
#include <memory>

#include "InterfaceConfigManager.hpp"
#include "IEthernetInterfaceFactory.hpp"
#include "InterfaceConfigManagerBaseTest.h"

#include "MockINetDevManager.hpp"
#include "MockIEthernetInterface.hpp"

namespace netconf {

using namespace ::std;
using namespace ::testing;

class InterfaceConfigManagerValidateTest : public InterfaceConfigManagerBaseTest, public Test {
 public:

  NiceMock<MockINetDevManager> netdev_manager_;
  NiceMock<MockIPersistencePortConfigs> persist_portconfig_mock_;
  unique_ptr<InterfaceConfigManager> sut_;

  unique_ptr<FakeEthernetInterfaceFactory> fake_fac_;

  NetDevs netdevs_;

  void SetUp() override
  {
    netdevs_.insert(netdevs_.begin(), {
        ::std::make_shared<NetDev>(0, "ethX1", "X1", DeviceType::Port),
        ::std::make_shared<NetDev>(0, "ethX2", "X2", DeviceType::Port), });

    ON_CALL(netdev_manager_, GetPortNetDevs() ).WillByDefault(Return(netdevs_));
    ON_CALL(netdev_manager_, GetNetDevs() ).WillByDefault(Return(netdevs_));
    ON_CALL(persist_portconfig_mock_, Write(_)).WillByDefault(Return(Status { StatusCode::OK }));
    ON_CALL(persist_portconfig_mock_, Read(_)).WillByDefault(Return(Status { StatusCode::PERSISTENCE_READ }));

    fake_fac_ = make_unique<FakeEthernetInterfaceFactory>(*this);
  }

  void InstantiateSut() {
    sut_ = make_unique<InterfaceConfigManager>(netdev_manager_, persist_portconfig_mock_, *fake_fac_);
    sut_->InitializePorts();
    EXPECT_EQ(netdevs_.size(), created_ethernet_interfaces.size());
  }

};

TEST_F(InterfaceConfigManagerValidateTest, ValidatePortConfiguration) {

  EXPECT_CALL(persist_portconfig_mock_, Read(_)).WillOnce(Return(Status { StatusCode::OK }));

  InstantiateSut();

  Status status;
  InterfaceConfigs configs;

  configs = InterfaceConfigs { { "X1", InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::FULL } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  configs = InterfaceConfigs { { "X0815", InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::FULL } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::INTERFACE_NOT_EXISTING, status.GetStatusCode());

  configs = InterfaceConfigs { { "X1", InterfaceState::UP, Autonegotiation::OFF, 777, Duplex::FULL } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::LINK_MODE_NOT_SUPPORTED, status.GetStatusCode());

  /* The user is allowed to set only speed or duplex, if auto negotiation is off */

  configs = InterfaceConfigs { { "X1", InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::UNKNOWN } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  configs = InterfaceConfigs { { "X1", InterfaceState::UP, Autonegotiation::OFF, -1, Duplex::FULL } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  /* If Autoneg is on, speed and duplex are disregarded */

  configs = InterfaceConfigs { { "X1", InterfaceState::UP, Autonegotiation::ON, -1, Duplex::UNKNOWN } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  configs = InterfaceConfigs { { "X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::UNKNOWN } };
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

}

}  // namespace netconf
