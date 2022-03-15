// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "HostnameController.hpp"
#include "DefaultHostname.hpp"

namespace netconf {
namespace api {


TEST(Hostname_Target, SetGetHostnameAndGetDefaultHostname){


  auto hostname = GetKernelHostname();
  auto default_hostname = GetDefaultHostname();
  EXPECT_EQ(hostname, default_hostname);

  ::std::string expected_hostname = hostname + "TEST";
  SetKernelHostname(expected_hostname);
  auto actual_hostname = GetKernelHostname();
  EXPECT_EQ(actual_hostname,expected_hostname);

  SetKernelHostname(default_hostname);

}



}  // namespace api
}  // namespace netconf
