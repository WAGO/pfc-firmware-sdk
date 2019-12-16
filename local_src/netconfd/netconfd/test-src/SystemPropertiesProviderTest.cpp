// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     SystemPropertiesProviderTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "SystemPropertiesProvider.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace netconfd {

class ASystemPropertiesProvider : public Test {
 public:
  ASystemPropertiesProvider() {
    system_properties_provider_ = ::std::make_unique<SystemPropertiesProvider>();
  }

  void SetUp() override {
  }

  void TearDown() override {

  }

  ::std::unique_ptr<SystemPropertiesProvider> system_properties_provider_;

};

TEST_F(ASystemPropertiesProvider, GetsTheHostname) {
  ::std::string hostname;
  Status status = system_properties_provider_->GetHostname(hostname);

  ASSERT_EQ(StatusCode::OK, status.Get());
  EXPECT_FALSE(hostname.empty());
}

} /* namespace netconfd */
