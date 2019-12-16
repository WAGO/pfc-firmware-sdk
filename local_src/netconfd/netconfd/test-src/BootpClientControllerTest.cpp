// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     BootpControllerTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <BootpClientController.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace netconfd {

class ABootpController_Target : public Test {
 public:
  ABootpController_Target() {

  }

  void SetUp() override {
  }

  void TearDown() override {

  }

  BootpClientController bootp_client_controller_;

};

TEST_F(ABootpController_Target, TestName) {
  bootp_client_controller_.GetStatus("br0");
}

} /* namespace netconfd */
