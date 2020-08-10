// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     InterfaceValidatorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "BridgeConfigValidator.hpp"
#include "MockIBridgeController.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

using namespace testing;

namespace netconf {

class AnInterfaceValidator : public Test {
 public:
  AnInterfaceValidator() {

  }

  void SetUp() override {
    validator_ = ::std::make_unique<BridgeConfigValidator>(
        mock_bridge_controller_);
  }

  void TearDown() override {

  }
  MockIBridgeController mock_bridge_controller_;
  ::std::unique_ptr<BridgeConfigValidator> validator_;
};

// Disabled: Do more strict validation as long as network-interfaces.xml generation is supported.
//
TEST_F(AnInterfaceValidator, DISABLED_checksAValidConfigurationContainingOneBridge) {

  BridgeConfig os_config { { "br0", { "ethX1" } } };

  Interfaces system_interfaces = { "ethX1" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnInterfaceValidator, checksAValidConfigurationContainingTwoBridges) {

  BridgeConfig os_config { { "br0", { "ethX1" } },
      { "br1", { "ethX2", "ethX3" } } };

  Interfaces system_interfaces = { "ethX1", "ethX2", "ethX3" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationMissingInterface) {

  BridgeConfig os_config { { "br0", { "ethX1", "ethX2", "ethX11" } }, { "br1",
      { } } };

  Interfaces system_interfaces = { "ethX1", "ethX2" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationContainingMultipleBridgesMissingInterface) {

  BridgeConfig os_config { { "br0", { "ethX1" } }, { "br1",
      { "ethX12", "ethX2" } }, { "br2", { "ethX3" } } };

  Interfaces system_interfaces = { "ethX1", "ethX2", "ethX3" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationInterfacesAssignedServeralTimes) {

  BridgeConfig os_config { { "br0", { "ethX1" } },
      { "br1", { "ethX1", "ethX2" } } };

  Interfaces system_interfaces = { "ethX1", "ethX2" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationInterfacesAssignedServeralTimesToSameBridge) {

  BridgeConfig os_config { { "br0", { "ethX1", "ethX1" } },
      { "br1", { "ethX2" } } };

  Interfaces system_interfaces = { "ethX1", "eth2" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnInterfaceValidator, checksAnInvalidBridgeNameContainingBlanks) {

  BridgeConfig os_config { { "br0 1", { "ethX3" } }, { "br0", { "ethX1" } }, {
      "br1", { "ethX2" } } };

  Interfaces system_interfaces = { "ethX1", "ethX2", "ethX3" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

TEST_F(AnInterfaceValidator, checksMaximumBridgeNameLengthOf15Characters) {

  BridgeConfig os_config { { "br0123456789123", { "ethX3" } }, { "br0", {
      "ethX1" } }, { "br1", { "ethX2" } } };

  Interfaces system_interfaces = { "ethX1", "ethX2", "ethX3" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnInterfaceValidator, checksMaximumBridgeNameLengthGreaterThan15Characters) {

  BridgeConfig os_config { { "br01234567891234", { "ethX3" } }, { "br0", { "ethX1" } }, {
      "br1", { "ethX2" } } };

  Interfaces system_interfaces = { "ethX1", "ethX2", "ethX3" };
  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(
      Return(system_interfaces));

  Status status = validator_->Validate(os_config);

  EXPECT_EQ(StatusCode::ERROR, status.Get());
}

} /* namespace netconf */
