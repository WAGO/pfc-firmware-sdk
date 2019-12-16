// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MacDistributorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "MacDistributor.hpp"
#include "MockIDevicePropertiesProvider.hpp"
#include "CommandExecutor.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>

using namespace testing;

namespace netconfd {

class AMacDistributor_Target : public Test {
 public:

  MockIDevicePropertiesProvider mock_properties_provider_;
  ::std::shared_ptr<MacDistributor> mac_distributor_;

  ::std::string const test_tap1_ = "tap1";
  ::std::string const test_tap2_ = "tap2";
  ::std::string const test_bridge1_ = "brtest1";
  ::std::string const test_bridge2_ = "brtest2";

  ::std::string const mac0_ = "00:30:42:11:11:A0";
  ::std::string const mac1_ = "00:30:42:11:11:A1";
  ::std::string const mac2_ = "00:30:42:11:11:A2";

  AMacDistributor_Target() {

  }

  void SetUp() override {
    AddTunInterface(test_tap1_);
    AddTunInterface(test_tap2_);
    AddBridge(test_bridge1_);
    AddBridge(test_bridge2_);
  }

  void TearDown() override {
    DeleteTunInterface(test_tap1_);
    DeleteTunInterface(test_tap2_);
    DeleteBridge(test_bridge1_);
    DeleteBridge(test_bridge2_);
  }

  void AddTunInterface(::std::string name) {
    FILE* file;
    file = popen(("ip tuntap add mode tap dev " + name).c_str(), "r");
    pclose(file);
  }

  void DeleteTunInterface(::std::string name) {
    FILE* file;
    file = popen(("ip tuntap del mode tap dev " + name).c_str(), "r");
    pclose(file);
  }

  void AddBridge(::std::string name) {
    FILE* file;
    file = popen(("brctl addbr " + name).c_str(), "r");
    pclose(file);
  }

  void DeleteBridge(::std::string name) {
    FILE* file;
    file = popen(("brctl delbr " + name).c_str(), "r");
    pclose(file);
  }

  ::std::string GetMacOf(::std::string const& interface) {

    struct ifreq ifr { };
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd != -1) {
      ioctl(socket_fd, SIOCGIFHWADDR, &ifr);
      close(socket_fd);
    }

    std::array<char, 19> mac_buffer { };

    snprintf(mac_buffer.data(), mac_buffer.size(), "%02X:%02X:%02X:%02X:%02X:%02X",
             ifr.ifr_hwaddr.sa_data[0], ifr.ifr_hwaddr.sa_data[1],
             ifr.ifr_hwaddr.sa_data[2], ifr.ifr_hwaddr.sa_data[3],
             ifr.ifr_hwaddr.sa_data[4], ifr.ifr_hwaddr.sa_data[5]);

    return mac_buffer.data();
  }

};

TEST_F(AMacDistributor_Target, AssignsMacAdressesToInterfacesInCasePropertyProviderSupportsSeveralMac) {

  Interfaces os_interfaces = { test_tap1_, test_tap2_ };

  EXPECT_CALL(mock_properties_provider_, GetOSInterfaces()).WillRepeatedly(
      Return(os_interfaces));
  EXPECT_CALL(mock_properties_provider_, GetIncrementedMac(1)).WillOnce(Return(mac1_));
  EXPECT_CALL(mock_properties_provider_, GetIncrementedMac(2)).WillOnce(Return(mac2_));

  mac_distributor_ = ::std::make_shared<MacDistributor>(mock_properties_provider_);

  EXPECT_EQ(mac1_, GetMacOf(test_tap1_));
  EXPECT_EQ(mac2_, GetMacOf(test_tap2_));
}

TEST_F(AMacDistributor_Target, AssignsMacAdressesToInterfacesInCasePropertyProviderSupportsOneMac) {

  Interfaces os_interfaces = { test_tap1_, test_tap2_ };

  EXPECT_CALL(mock_properties_provider_, GetOSInterfaces()).WillRepeatedly(
      Return(os_interfaces));
  EXPECT_CALL(mock_properties_provider_, GetIncrementedMac(_)).WillRepeatedly(Return(""));
  EXPECT_CALL(mock_properties_provider_, GetMac()).WillRepeatedly(Return(mac1_));

  mac_distributor_ = ::std::make_shared<MacDistributor>(mock_properties_provider_);

  EXPECT_EQ(mac1_, GetMacOf(test_tap1_));
  EXPECT_EQ(mac1_, GetMacOf(test_tap2_));
}

TEST_F(AMacDistributor_Target, TriesToAssignMacAdressesAlthoughTheProviderProvidesNoneInterface) {

  Interfaces os_interfaces = { };
  EXPECT_CALL(mock_properties_provider_, GetOSInterfaces()).WillRepeatedly(
      Return(os_interfaces));
  EXPECT_CALL(mock_properties_provider_, GetIncrementedMac(_)).Times(0);

  mac_distributor_ = ::std::make_shared<MacDistributor>(mock_properties_provider_);

}

TEST_F(AMacDistributor_Target, TriesToAssignMacAdressesAlthoughTheOsInterfaceDoesNotExist) {

  DeleteTunInterface(test_tap1_);

  Interfaces os_interfaces = { test_tap1_ };
  EXPECT_CALL(mock_properties_provider_, GetOSInterfaces()).WillRepeatedly(
      Return(os_interfaces));
  EXPECT_CALL(mock_properties_provider_, GetIncrementedMac(1)).WillOnce(Return(mac1_));

  EXPECT_THROW(::std::make_shared<MacDistributor>(mock_properties_provider_), ::std::runtime_error);

}

TEST_F(AMacDistributor_Target, AssignsMacAdressesToBridges) {

  mac_distributor_ = ::std::make_shared<MacDistributor>(mock_properties_provider_);

  EXPECT_CALL(mock_properties_provider_, GetMac()).WillRepeatedly(Return(mac0_));

  mac_distributor_->SetMac(test_bridge1_);
  mac_distributor_->SetMac(test_bridge2_);

  EXPECT_EQ(mac0_, GetMacOf(test_bridge1_));
  EXPECT_EQ(mac0_, GetMacOf(test_bridge2_));
}

TEST_F(AMacDistributor_Target, TriesToSetMacAdressesWhileThePropertiesProviderProvidesNoMac) {

  mac_distributor_ = ::std::make_shared<MacDistributor>(mock_properties_provider_);

  EXPECT_CALL(mock_properties_provider_, GetMac()).WillRepeatedly(Return(""));

  Status status = mac_distributor_->SetMac(test_bridge1_);

  EXPECT_EQ(StatusCode::ERROR, status.Get());

}

TEST_F(AMacDistributor_Target, TriesToSetMacAdressesAlthoughTheBridgeDoesNotExist) {

  DeleteBridge(test_bridge1_);

  mac_distributor_ = ::std::make_shared<MacDistributor>(mock_properties_provider_);

  EXPECT_CALL(mock_properties_provider_, GetMac()).WillRepeatedly(Return(mac0_));

  Status status = mac_distributor_->SetMac(test_bridge1_);

  EXPECT_EQ(StatusCode::SYSTEM_CALL_ERROR, status.Get());

}


} /* namespace netconfd */
