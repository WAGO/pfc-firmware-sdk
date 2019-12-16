// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IPManagerTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "IPManager.hpp"
#include "MockIDevicePropertiesProvider.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MockIInterfaceInformation.hpp>
#include <memory>

using namespace testing;

namespace netconfd {

class AnIPManager : public Test {
 public:

  MockIDevicePropertiesProvider mock_properties_provider_;
  MockIInterfaceManagerInformation mock_interface_manager_;

  AnIPManager() {

  }

  void SetUp() override {
    ip_manager_ = ::std::make_unique<IPManager>(mock_properties_provider_, mock_interface_manager_);
  }

  void TearDown() override {

  }

 private:
  ::std::unique_ptr<IPManager> ip_manager_;
};

TEST_F(AnIPManager, TestName) {

}

} /* namespace netconfd */
