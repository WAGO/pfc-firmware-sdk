// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "HostnameController.hpp"
#include "DefaultHostname.hpp"

namespace netconf {
namespace api {


TEST(DefaultHostname, GetDefaultHostname){

  ::std::string expected = "PFC200V3-425DE4";

  auto default_hostname = GetDefaultHostname("pfc200v3\n", "00:30:de:42:5d:e4\n");
  EXPECT_EQ(expected, default_hostname);

}



}  // namespace api
}  // namespace netconf
