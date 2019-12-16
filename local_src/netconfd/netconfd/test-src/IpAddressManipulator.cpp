//------------------------------------------------------------------------------


#include <gtest/gtest.h>

#include "IpAddressManipulator.hpp"


namespace netconfd {

class AnIpAddressManipulator : public ::testing::Test
{
  public:

  IPConfig test_config;

  void SetAddressToConfig(const ::std::string& address)
  {
    test_config.address_ = address;
  }

  void SetNetmaskToConfig(const ::std::string& netmask)
  {
    test_config.netmask_ = netmask;
  }
};


TEST_F(AnIpAddressManipulator, DoesNotChangeEmptyStringAddress)
{

  auto new_config = IpAddressManipulator::ChangeHost(test_config, 123);

  EXPECT_STREQ("", new_config.address_.c_str());

}

TEST_F(AnIpAddressManipulator, DoesNotChangeZeroAddress)
{

  SetAddressToConfig("0.0.0.0");
  SetNetmaskToConfig("0.0.0.0");


  auto new_config = IpAddressManipulator::ChangeHost(test_config, 123);

  EXPECT_STREQ("0.0.0.0", new_config.address_.c_str());

}

TEST_F(AnIpAddressManipulator, ChangesHostPartOfClassCAddress)
{

  SetAddressToConfig("192.168.5.42");
  SetNetmaskToConfig("255.255.255.0");


  auto new_config = IpAddressManipulator::ChangeHost(test_config, 123);

  EXPECT_STREQ("192.168.5.123", new_config.address_.c_str());

}

TEST_F(AnIpAddressManipulator, ChangesHostPartOfSmallSubnet)
{

  SetAddressToConfig("192.168.5.5");
  SetNetmaskToConfig("255.255.255.240");


  auto new_config = IpAddressManipulator::ChangeHost(test_config, 123);

  EXPECT_STREQ("192.168.5.11", new_config.address_.c_str());

}

TEST_F(AnIpAddressManipulator, ChangesHostPartOfBiggerSubnet)
{

  SetAddressToConfig("192.168.5.5");
  SetNetmaskToConfig("255.255.128.0");


  auto new_config = IpAddressManipulator::ChangeHost(test_config, 123);

  EXPECT_STREQ("192.168.0.123", new_config.address_.c_str());

}

}  // namespace netconfd


//---- End of source file ------------------------------------------------------

