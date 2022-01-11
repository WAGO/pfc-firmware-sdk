// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DevicePropertiesQuerierTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <DeviceProperties.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace netconf {

class ADevicePropertiesProvider : public Test {
 public:
  ADevicePropertiesProvider() {

  }

  void SetUp() override {
  }

  void TearDown() override {

  }

};

TEST_F(ADevicePropertiesProvider, TestName) {

}

} /* namespace netconf */
