// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>

#include "DSAModeHandler.hpp"
#include "MockBridgeConfig.hpp"
#include "Error.hpp"
#include <gmock/gmock.h>

using namespace testing;

using namespace netconf::api;

namespace po = boost::program_options;
using povm = boost::program_options::variables_map;

namespace network_config {

static povm CreateCommandLine(::std::string cmd) {
  povm map_;
  ::std::stringstream ss;
  ss << cmd;

  po::options_description desc("");
  desc.add_options()("set", "set the mode")("get", "get the mode");
  po::store(po::parse_config_file(ss, desc), map_);
  return map_;
}


TEST(DSAModeHandlerTest, GetAConfig)
{
  auto opt_map = CreateCommandLine("get=1\n");

  DSAModeHandler dut{opt_map};
  MockBridgeConfig mbc;
  BridgeConfig returned_config;
  MakeBridgeConfig(R"({"br0":["X1"],"br1":["X2"]})", returned_config);
  EXPECT_CALL(mbc, GetBridgeConfig(_)).WillOnce(DoAll(SetArgReferee<0>(returned_config), Return(netconf::Error::Ok())));

  dut.Execute();

}

TEST(DSAModeHandlerTest, SetDSAModeNoChange)
{
  auto opt_map = CreateCommandLine("set=1\n");

  DSAModeHandler dut{opt_map};
  MockBridgeConfig mbc;
  BridgeConfig returned_config;
  BridgeConfig expected_config;
  MakeBridgeConfig(R"({"br0":["X1"],"br1":["X2"]})",returned_config);
  MakeBridgeConfig(R"({"br0":["X1"],"br1":["X2"]})",expected_config);
  EXPECT_CALL(mbc, GetBridgeConfig(_)).WillOnce(DoAll(SetArgReferee<0>(returned_config), Return(netconf::Error::Ok())));
  BridgeConfig set_parameter;
  EXPECT_CALL(mbc, SetBridgeConfig(_)).WillOnce(DoAll(SaveArg<0>(&set_parameter), Return(netconf::Error::Ok())));

  dut.Execute();

  EXPECT_EQ(ToString(set_parameter),ToString(expected_config));
}

TEST(DSAModeHandlerTest, SetDSAModeWithChange)
{
  auto opt_map = CreateCommandLine("set=0\n");

  DSAModeHandler dut{opt_map};
  MockBridgeConfig mbc;
  BridgeConfig returned_config;
  BridgeConfig expected_config;
  MakeBridgeConfig(R"({"br0":["X1"],"br1":["X2"]})", returned_config);
  MakeBridgeConfig(R"({"br0":["X1", "X2"]})", expected_config);
  EXPECT_CALL(mbc, GetBridgeConfig(_)).WillOnce(DoAll(SetArgReferee<0>(returned_config), Return(netconf::Error::Ok())));
  BridgeConfig set_parameter;
  EXPECT_CALL(mbc, SetBridgeConfig(_)).WillOnce(DoAll(SaveArg<0>(&set_parameter), Return(netconf::Error::Ok())));

  dut.Execute();

  EXPECT_EQ(ToString(set_parameter),ToString(expected_config));
}

TEST(DSAModeHandlerTest, SetDSAModeOtherInterfaceRemains)
{
  auto opt_map = CreateCommandLine("set=0\n");

  DSAModeHandler dut{opt_map};
  MockBridgeConfig mbc;
  BridgeConfig returned_config;
  BridgeConfig expected_config;
  MakeBridgeConfig(R"({"br0":["X1"],"br1":["X2", "X11"]})",returned_config);
  MakeBridgeConfig(R"({"br0":["X1", "X2"],"br1":["X11"]})",expected_config);
  EXPECT_CALL(mbc, GetBridgeConfig(_)).WillOnce(DoAll(SetArgReferee<0>(returned_config), Return(netconf::Error::Ok())));
  BridgeConfig set_parameter;
  EXPECT_CALL(mbc, SetBridgeConfig(_)).WillOnce(DoAll(SaveArg<0>(&set_parameter), Return(netconf::Error::Ok())));

  dut.Execute();

  EXPECT_EQ(ToString(set_parameter),ToString(expected_config));
}



}  // namespace netconf
