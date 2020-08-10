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



static std::ostream& operator<<(std::ostream& os, const InterfaceConfig& pc) {
  os << "{Device: " << pc.device_name_ << ", ";
  os << "State: " << static_cast<int>(pc.state_) << ", ";
  os << "Autoneg: " << static_cast<int>(pc.autoneg_) << ", ";
  os << "Speed: " << pc.speed_ << ", ";
  os << "Duplex: " << static_cast<int>(pc.duplex_) << "}";
  return os;
}

class InterfaceConfigManagerPersistenceTest : public InterfaceConfigManagerBaseTest, public Test {
 public:

  MockINetDevManager netdev_manager_;
  MockIPersistencePortConfigs persist_portconfig_mock;
  MockIEthernetInterface ethernet_interface_mock;
  unique_ptr<InterfaceConfigManager> sut;

  unique_ptr<FakeEthernetInterfaceFactory> fake_fac_;

  NetDevs netdevs_;

  InterfaceConfigs persisted_matching_port_config;
  InterfaceConfigs persisted_missing_port_config;
  InterfaceConfigs persisted_oversized_port_config;

  InterfaceConfigs expected_port_config_oversized;
  InterfaceConfigs expected_port_config_missing;

  InterfaceConfigs new_port_configs_full;

  InterfaceConfigs new_port_configs_partial;
  InterfaceConfigs persisted_new_port_configs_partial;

  /*
   ::std::string device_name_;
   InterfaceState state_;
   Autonegotiation autoneg_;
   int speed_;
   Duplex duplex_;
   */
  void SetUp() override
  {
    persisted_matching_port_config.emplace_back("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);
    persisted_matching_port_config.emplace_back("X2", InterfaceState::UP, Autonegotiation::OFF, 1000, Duplex::HALF);

    persisted_missing_port_config.emplace_back("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);

    expected_port_config_missing.emplace_back("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);
    expected_port_config_missing.emplace_back("X2", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);

    persisted_oversized_port_config.emplace_back("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);
    persisted_oversized_port_config.emplace_back("X2", InterfaceState::UP, Autonegotiation::OFF, 1000, Duplex::HALF);
    persisted_oversized_port_config.emplace_back("X12", InterfaceState::UP, Autonegotiation::ON, 10, Duplex::FULL);
    persisted_oversized_port_config.emplace_back("X11", InterfaceState::DOWN, Autonegotiation::OFF, 100, Duplex::FULL);

    expected_port_config_oversized.emplace_back("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);
    expected_port_config_oversized.emplace_back("X2", InterfaceState::UP, Autonegotiation::OFF, 1000, Duplex::HALF);

    new_port_configs_partial.emplace_back("X2", InterfaceState::UP, Autonegotiation::ON, 10, Duplex::FULL);

    persisted_new_port_configs_partial.emplace_back("X1", InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL);
    persisted_new_port_configs_partial.emplace_back("X2", InterfaceState::UP, Autonegotiation::ON, 10, Duplex::FULL);

    new_port_configs_full.emplace_back("X1", InterfaceState::DOWN, Autonegotiation::OFF, 1000, Duplex::HALF);
    new_port_configs_full.emplace_back("X2", InterfaceState::DOWN, Autonegotiation::ON, 10, Duplex::FULL);

    netdevs_.insert(netdevs_.begin(), {
        ::std::make_shared<NetDev>(0,"ethX1", "X1", DeviceType::Ethernet),
         ::std::make_shared<NetDev>(0,"ethX2", "X2", DeviceType::Ethernet),
    });

    fake_fac_ = make_unique<FakeEthernetInterfaceFactory>(*this);
  }

  void InstantiateSut() {
    sut = make_unique<InterfaceConfigManager>(netdev_manager_, persist_portconfig_mock,
                                              *fake_fac_);
    sut->InitializePorts();
    EXPECT_EQ(netdevs_.size(), created_ethernet_interfaces.size());
  }

  }
  ;

// TEST list
// 1. Starten ohne Persistenz Datei
// 2. Starten mit passender Persistenz Datei
// 3. Starten mit zu vielen PortConfigs in der Persistenz Datei
// 4. Starten mit zu wenig PortConfigs in der Persistenz Datei
// 6. Änderungen werden Persistiert

  TEST_F(InterfaceConfigManagerPersistenceTest, StartWithoutPersistenceData) {
    EXPECT_CALL(netdev_manager_, GetPortNetDevs() ).WillOnce(Return(netdevs_));
    EXPECT_CALL(persist_portconfig_mock, Read(_)).WillOnce(Return(Status { StatusCode::FILE_READ_ERROR }));

    InstantiateSut();

    auto itf_X1 = FindCreatedEthernetInterface("ethX1");
    auto itf_X2 = FindCreatedEthernetInterface("ethX2");

    ASSERT_THAT(itf_X1, NotNull());
    ASSERT_THAT(itf_X2, NotNull());

    EXPECT_TRUE(itf_X1->committed_);
    EXPECT_TRUE(itf_X2->committed_);

    EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
    EXPECT_EQ(eth::Autoneg::On, itf_X2->autoneg_);

    EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
    EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  }

  ACTION_P(FillPortConfigs, port_configs){
  InterfaceConfigs& dst_port_cfg = arg0;
  dst_port_cfg.insert(dst_port_cfg.begin(), port_configs.begin(), port_configs.end());
}

TEST_F(InterfaceConfigManagerPersistenceTest, StartWithMatchingPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetPortNetDevs() ).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_)).WillOnce(
      DoAll(FillPortConfigs(persisted_matching_port_config), Return(Status { StatusCode::OK })));

  InstantiateSut();

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::Off, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  EXPECT_EQ(100, itf_X1->speed_);
  EXPECT_EQ(1000, itf_X2->speed_);

  EXPECT_EQ(eth::Duplex::Full, itf_X1->duplex_);
  EXPECT_EQ(eth::Duplex::Half, itf_X2->duplex_);
}

TEST_F(InterfaceConfigManagerPersistenceTest, StartWithOversizedPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetPortNetDevs() ).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_)).WillOnce(
      DoAll(FillPortConfigs(persisted_oversized_port_config), Return(Status { StatusCode::OK })));

  InstantiateSut();

  EXPECT_THAT(sut->GetPortConfigs(), ContainerEq(expected_port_config_oversized));

  EXPECT_EQ(2u, created_ethernet_interfaces.size());

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::Off, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  EXPECT_EQ(100, itf_X1->speed_);
  EXPECT_EQ(1000, itf_X2->speed_);

  EXPECT_EQ(eth::Duplex::Full, itf_X1->duplex_);
  EXPECT_EQ(eth::Duplex::Half, itf_X2->duplex_);
}

TEST_F(InterfaceConfigManagerPersistenceTest, StartMissingPortConfigsInPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetPortNetDevs() ).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_)).WillOnce(
      DoAll(FillPortConfigs(persisted_missing_port_config), Return(Status { StatusCode::OK })));

  InstantiateSut();

  EXPECT_THAT(sut->GetPortConfigs(), ContainerEq(expected_port_config_missing));

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::On, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  EXPECT_EQ(100, itf_X1->speed_);
  EXPECT_EQ(100, itf_X2->speed_);

  EXPECT_EQ(eth::Duplex::Full, itf_X1->duplex_);
  EXPECT_EQ(eth::Duplex::Full, itf_X2->duplex_);
}

ACTION_P(CopyToVector, vector){
std::copy(arg0.begin(), arg0.end(), std::back_inserter(vector.get()));
}

TEST_F(InterfaceConfigManagerPersistenceTest, PersistPartialNewConfig) {
  EXPECT_CALL(netdev_manager_, GetPortNetDevs() ).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_)).WillOnce(
      DoAll(FillPortConfigs(persisted_matching_port_config), Return(Status { StatusCode::OK })));

  InstantiateSut();

  InterfaceConfigs written_configs;
  EXPECT_CALL(persist_portconfig_mock, Write(_)).WillOnce(DoAll(CopyToVector(std::ref(written_configs)), Return(Status {
      StatusCode::OK })));
  sut->Configure(new_port_configs_partial);

  EXPECT_THAT(written_configs, ContainerEq(persisted_new_port_configs_partial));

  written_configs.clear();

  EXPECT_CALL(persist_portconfig_mock, Write(_)).WillOnce(DoAll(CopyToVector(std::ref(written_configs)), Return(Status {
      StatusCode::OK })));
  sut->Configure(new_port_configs_full);
  EXPECT_THAT(written_configs, ContainerEq(new_port_configs_full));

}

}  // namespace netconf
