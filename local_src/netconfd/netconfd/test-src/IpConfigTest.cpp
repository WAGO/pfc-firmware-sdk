

#include <gtest/gtest.h>

#include "Types.hpp"

namespace netconfd {

TEST(IpConfigTest, SourceChanges)
{
  IPConfig old;
  IPConfig new_;
  IPConfig new2_;
  new2_.source_ = IPSource::STATIC;

  EXPECT_FALSE(IPConfig::SourceChangesToAnyOf(old, new_));
  EXPECT_TRUE(IPConfig::SourceChangesToAnyOf(old, new2_));

}

TEST(IpConfigTest, SourceChangesToAnyOf)
{
  IPConfig old;
  IPConfig new_;
  new_.source_ = IPSource::STATIC;

  EXPECT_TRUE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::STATIC));
  EXPECT_TRUE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::STATIC, IPSource::STATIC));
  EXPECT_TRUE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::STATIC, IPSource::BOOTP));
  EXPECT_TRUE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::BOOTP, IPSource::STATIC));
  EXPECT_TRUE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::BOOTP, IPSource::DHCP, IPSource::STATIC));
  EXPECT_FALSE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::DHCP, IPSource::BOOTP));

}

TEST(IpConfigTest, SourceDoesNotChange)
{
  IPConfig old;
  old.source_ = IPSource::STATIC;
  IPConfig new_;
  new_.source_ = IPSource::STATIC;

  EXPECT_FALSE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::STATIC));
  EXPECT_FALSE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::STATIC, IPSource::BOOTP));
  EXPECT_FALSE(IPConfig::SourceChangesToAnyOf(old, new_, IPSource::DHCP, IPSource::BOOTP));

}

}  // namespace netconfd




