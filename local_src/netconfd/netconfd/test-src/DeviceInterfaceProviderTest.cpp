// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DeviceSettingsTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <DeviceInterfaceProvider.hpp>
#include "Helper.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "MockIBridgeController.hpp"

using namespace testing;

namespace netconfd {

class ADeviceInterfaceProvider : public Test {
 public:

  MockIBridgeController mock_bridge_controller_;
  ::std::unique_ptr<DeviceInterfaceProvider> interface_provider_;

  ADeviceInterfaceProvider() {

  }

  void SetUp() override {

  }

  void TearDown() override {

  }

};

TEST_F(ADeviceInterfaceProvider, GeneratesTheDeviceSpecificProductPortInterfaceList) {

  Interfaces interfaces = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(interfaces));

  interface_provider_ = ::std::make_unique < DeviceInterfaceProvider
      > (mock_bridge_controller_);

  Interfaces product_interfaces = interface_provider_->GetProductInterfaces();

  EXPECT_TRUE(IsIncluded("X1", product_interfaces));
  EXPECT_TRUE(IsIncluded("X2", product_interfaces));
  EXPECT_EQ(2, product_interfaces.size());
}

TEST_F(ADeviceInterfaceProvider, GeneratesTheDeviceSpecificOSPortInterfaceList) {

  Interfaces interfaces = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(interfaces));

  interface_provider_ = ::std::make_unique < DeviceInterfaceProvider
      > (mock_bridge_controller_);

  Interfaces os_interfaces = interface_provider_->GetOSInterfaces();

  EXPECT_TRUE(IsIncluded("ethX1", os_interfaces));
  EXPECT_TRUE(IsIncluded("ethX2", os_interfaces));
  EXPECT_EQ(2, os_interfaces.size());
}

TEST_F(ADeviceInterfaceProvider, GeneratesTheDeviceSpecificInterfaceNameMapping) {

  Interfaces interfaces = { "ethX1", "ethX2" };

  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(interfaces));

  interface_provider_ = ::std::make_unique < DeviceInterfaceProvider
      > (mock_bridge_controller_);

  InterfaceNameMapping name_mapping = interface_provider_->GetInterfacesNameMapping();

  EXPECT_EQ("ethX1", name_mapping.at("X1"));
  EXPECT_EQ("ethX2", name_mapping.at("X2"));

  EXPECT_EQ(2, name_mapping.size());
}

TEST_F(ADeviceInterfaceProvider, GeneratesTheDeviceSpecificProductPortInterfaceListIgnoreUnimportendInterfaces) {

  Interfaces interfaces = { "wwan0", "ethX1", "abcethX7", "ethX110", "X22", "X1",
      "eth", "_ethX3", "eth0" , "", "ethX2", "ethXabc", "ethx42"};

  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(interfaces));

  interface_provider_ = ::std::make_unique < DeviceInterfaceProvider
      > (mock_bridge_controller_);

  Interfaces product_interfaces = interface_provider_->GetProductInterfaces();

  EXPECT_TRUE(IsIncluded("X1", product_interfaces));
  EXPECT_TRUE(IsIncluded("X2", product_interfaces));
  EXPECT_TRUE(IsIncluded("X110", product_interfaces));
  EXPECT_TRUE(IsIncluded("Xabc", product_interfaces));
  EXPECT_EQ(4, product_interfaces.size());
}

TEST_F(ADeviceInterfaceProvider, GeneratesTheDeviceSpecificOSPortInterfaceListIgnoreUnimportendInterfaces) {

  Interfaces interfaces = { "wwan0", "ethX1", "abcethX7", "ethX110", "X22", "X1",
      "eth", "_ethX3", "eth0" , "", "ethX2", "ethXabc", "ethx42"};

  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(interfaces));

  interface_provider_ = ::std::make_unique < DeviceInterfaceProvider
      > (mock_bridge_controller_);

  Interfaces os_interfaces = interface_provider_->GetOSInterfaces();

  EXPECT_TRUE(IsIncluded("ethX1", os_interfaces));
  EXPECT_TRUE(IsIncluded("ethX2", os_interfaces));
  EXPECT_TRUE(IsIncluded("ethX110", os_interfaces));
  EXPECT_TRUE(IsIncluded("ethXabc", os_interfaces));
  EXPECT_EQ(4, os_interfaces.size());
}

TEST_F(ADeviceInterfaceProvider, GeneratesTheDeviceSpecificInterfaceNameMappingIgnoreUnimportendInterfaces) {

  Interfaces interfaces = { "wwan0", "ethX1", "abcethX7", "ethX110", "X22", "X1",
      "eth", "_ethX3", "eth0" , "", "ethX2", "ethXabc", "ethx42"};

  EXPECT_CALL(mock_bridge_controller_, GetInterfaces()).WillOnce(Return(interfaces));

  interface_provider_ = ::std::make_unique < DeviceInterfaceProvider
      > (mock_bridge_controller_);

  InterfaceNameMapping name_mapping = interface_provider_->GetInterfacesNameMapping();

  EXPECT_EQ("ethX1", name_mapping.at("X1"));
  EXPECT_EQ("ethX2", name_mapping.at("X2"));
  EXPECT_EQ("ethX110", name_mapping.at("X110"));
  EXPECT_EQ("ethXabc", name_mapping.at("Xabc"));

  EXPECT_EQ(4, name_mapping.size());
}

} /* namespace netconfd */
