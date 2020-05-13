// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IPControllerTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "IPController.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

using namespace testing;

namespace netconfd {

class AnIPController_Target : public Test {
 public:
  AnIPController_Target() {
    ip_controller_ = ::std::make_unique<IPController>();
  }

  void SetUp() override {
    AddBridge(bridge_);
  }

  void TearDown() override {
    DeleteBridge(bridge_);
  }

  void AddBridge(::std::string name) {
    FILE* file;
    file = popen(("brctl addbr " + name).c_str(), "r");
    pclose(file);
  }

  void DeleteBridge(::std::string name) {
    FILE* file;
    file = popen(("brctl delbr " + name).c_str(), "r");
    pclose(file);
  }

  void ExpectEQIPConfigsIgnoreSource(const IPConfig& lhs, const IPConfig& rhs) const {
    EXPECT_EQ(lhs.interface_, rhs.interface_);
    EXPECT_EQ(lhs.address_, rhs.address_);
    EXPECT_EQ(lhs.netmask_, rhs.netmask_);
    EXPECT_EQ(lhs.broadcast_, rhs.broadcast_);
  }

  const Bridge bridge_ = "br_test";
  ::std::unique_ptr<IPController> ip_controller_;

};

TEST_F(AnIPController_Target, SetsAnEmptyBroadcast) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255.0", "");
  Status status = ip_controller_->SetIPConfig(config);

  ASSERT_EQ(StatusCode::OK, status.Get());
}

TEST_F(AnIPController_Target, SetsAndGetsAnIPConfiguration) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255.0", "192.168.42.255");
  Status status = ip_controller_->SetIPConfig(config);

  ASSERT_EQ(StatusCode::OK, status.Get());

  IPConfig current_config;
  status = ip_controller_->GetIPConfig(bridge_, current_config);
  ASSERT_EQ(StatusCode::OK, status.Get());
  ExpectEQIPConfigsIgnoreSource(config, current_config);
}

TEST_F(AnIPController_Target, SetsAndGetsAnIPConfigurationWithoutSpecifyingABroadcast) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255.0");
  Status status = ip_controller_->SetIPConfig(config);

  ASSERT_EQ(StatusCode::OK, status.Get());

  IPConfig current_config;
  status = ip_controller_->GetIPConfig(bridge_, current_config);
  ASSERT_EQ(StatusCode::OK, status.Get());

  IPConfig complete_config = config;
  complete_config.broadcast_ = "192.168.42.255";
  ExpectEQIPConfigsIgnoreSource(complete_config, current_config);
}

TEST_F(AnIPController_Target, SetsAndGetsAnZeroIPConfiguration) {

  IPConfig config(bridge_, IPSource::STATIC,ZeroIP, ZeroIP, ZeroIP);
  Status status = ip_controller_->SetIPConfig(config);

  ASSERT_EQ(StatusCode::OK, status.Get());

  IPConfig current_config;
  status = ip_controller_->GetIPConfig(bridge_, current_config);
  ASSERT_EQ(StatusCode::OK, status.Get());

  ExpectEQIPConfigsIgnoreSource(current_config, current_config);
}

TEST_F(AnIPController_Target, SetsAndGetsAnZeroIPConfigurationWithoutSpecifyingABroadcast) {

  IPConfig config(bridge_, IPSource::STATIC,ZeroIP, ZeroIP);
  Status status = ip_controller_->SetIPConfig(config);

  ASSERT_EQ(StatusCode::OK, status.Get());

  IPConfig current_config;
  status = ip_controller_->GetIPConfig(bridge_, current_config);
  ASSERT_EQ(StatusCode::OK, status.Get());

  IPConfig complete_config = config;
  complete_config.broadcast_ = ZeroIP;
  ExpectEQIPConfigsIgnoreSource(complete_config, current_config);
}

TEST_F(AnIPController_Target, TriesToSetAnEmptyNetmask) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "", "192.168.42.255");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPController_Target, TriesToSetAnInvalidIPAddress) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.555", "255.255.255.0", "192.168.42.255");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPController_Target, TriesToSetAnInvalidNetmask) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255", "192.168.42.255");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPController_Target, TriesToSetAnInvalidBroadcast) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255.0", "192.256.42.255");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPController_Target, TriesToSetAnZeroIPAddress) {

  IPConfig config(bridge_, IPSource::STATIC,"", "255.255.255.0", "192.168.42.255");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::INVALID_PARAMETER, status.Get());
}

TEST_F(AnIPController_Target, GetsAnIpConfigFromAnUnconfiguredInterface) {

  IPConfig expected_config(bridge_, IPSource::NONE,ZeroIP, ZeroIP, ZeroIP);

  IPConfig config;

  Status status = ip_controller_->GetIPConfig(bridge_, config);

  ASSERT_EQ(StatusCode::OK, status.Get());
  ExpectEQIPConfigsIgnoreSource(expected_config, config);
}

TEST_F(AnIPController_Target, GetsAnIpConfigFromAMissingInterface) {

  IPConfig config;

  Status status = ip_controller_->GetIPConfig("missing_bridge", config);

  EXPECT_EQ(StatusCode::SYSTEM_CALL_ERROR, status.Get());
}


} /* namespace netconfd */
