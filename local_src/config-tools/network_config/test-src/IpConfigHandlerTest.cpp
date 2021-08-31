// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include "IPConfigHandler.hpp"
#include "IPConfigMock.hpp"

namespace network_config {

using namespace ::testing;

template<typename ... Args>
auto CreateProgramArgs(Args ... args) -> ::std::array<const char*, sizeof...(args)>
{
  return ::std::array<const char*, sizeof...(args)>{{args...}};
}

class IPConfigHandlerTest : public ::testing::Test {
 public:
  netconf::api::MockIPConfig ip_config_mock_;
  OptionParser opt_parser;

  void SetUp() override{

  }

  template<typename ... Args>
  IPConfigHandler CreateWithParams(Args ... args) {
    auto arg_array = CreateProgramArgs("program", args...);
    opt_parser.Parse(arg_array.size(), arg_array.data());
    return IPConfigHandler(opt_parser);
  }

};


TEST_F(IPConfigHandlerTest, SetWithBridgeDuplicateFails)
{
  auto sut = CreateWithParams("--ip-config","-fjson", "--set", R"( {"br0": {"bcast": "192.168.1.255","ipaddr": "192.168.1.17","netmask": "255.255.255.0","source": "static"},"br0": {"bcast": "192.168.255.255","ipaddr": "192.168.2.17","netmask": "255.255.0.0","source": "static"}})");
  EXPECT_CALL(ip_config_mock_, SetIPConfigs(_)).Times(0);
  EXPECT_ANY_THROW(sut.Execute());

}

TEST_F(IPConfigHandlerTest, SetWithTwoBridges)
{
  auto sut = CreateWithParams("--ip-config","-fjson", "--set", R"( {"br0": {"bcast": "192.168.1.255","ipaddr": "192.168.1.17","netmask": "255.255.255.0","source": "static"},"br1": {"bcast": "192.168.255.255","ipaddr": "192.168.2.17","netmask": "255.255.0.0","source": "static"}})");

  EXPECT_CALL(ip_config_mock_, SetIPConfigs(_)).WillOnce(Return(netconf::Status::Ok()));
  EXPECT_NO_THROW(sut.Execute());

}


}
