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

namespace netconf {

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
  }

  const Bridge bridge_ = "br_test";
  ::std::unique_ptr<IPController> ip_controller_;

};

TEST_F(AnIPController_Target, SetsAnEmptyBroadcast) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255.0");
  Status status = ip_controller_->SetIPConfig(config);

  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(AnIPController_Target, TriesToSetAnEmptyNetmask) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::SET_IP, status.GetStatusCode());
}

TEST_F(AnIPController_Target, TriesToSetAnInvalidIPAddress) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.555", "255.255.255.0");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::SET_IP, status.GetStatusCode());
}

TEST_F(AnIPController_Target, TriesToSetAnInvalidNetmask) {

  IPConfig config(bridge_, IPSource::STATIC,"192.168.42.42", "255.255.255");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::SET_IP, status.GetStatusCode());
}

TEST_F(AnIPController_Target, TriesToSetAnZeroIPAddress) {

  IPConfig config(bridge_, IPSource::STATIC,"", "255.255.255.0");
  Status status = ip_controller_->SetIPConfig(config);

  EXPECT_EQ(StatusCode::SET_IP, status.GetStatusCode());
}



} /* namespace netconf */
