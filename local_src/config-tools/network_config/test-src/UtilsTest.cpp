

#include <gtest/gtest.h>
#include "Utils.hpp"
#include "OptionStrings.hpp"

namespace network_config {

using namespace ::std::string_literals;

TEST(UtilsTest, JoinOptionNames)
{
  auto options = GetOptions();
  auto result_str = JoinOptionNames(" ", options.backup, options.device_info);

  auto expected = options.backup.name +" "s+ options.device_info.name;

  EXPECT_EQ(expected, result_str);
}

}
