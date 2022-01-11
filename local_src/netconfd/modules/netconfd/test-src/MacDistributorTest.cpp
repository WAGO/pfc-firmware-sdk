// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MacDistributorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "MacDistributor.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <unistd.h>

#include "MockIMacController.hpp"

namespace netconf {

using namespace std;
using namespace testing;
using namespace std::string_literals;

ACTION_P(PutToAssignmentList, list)
  {string itf {arg1};
  MacAddress mac {arg0};
  list.get().push_back( {itf, mac});
}

class AMacDistributor : public Test {
 public:

  MockIMacController mock_mac_controller_;

  unique_ptr<MacDistributor> mac_distributor_;
  NetDevs netdevs;

  MacAddress base_mac_ = MacAddress { { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 } };
  using AssigmentPair = std::pair<::std::string, ::netconf::MacAddress>;
  std::list<AssigmentPair> mac_assingment_list;

  NetDevPtr br0 = MakeNetDev(1, "br0", DeviceType::Bridge);
  NetDevPtr br1 = MakeNetDev(1, "br1", DeviceType::Bridge);
  NetDevPtr br2 = MakeNetDev(1, "br2", DeviceType::Bridge);
  NetDevPtr br3 = MakeNetDev(1, "br3", DeviceType::Bridge);
  NetDevPtr wwan0 = MakeNetDev(1, "wwan0", DeviceType::Wwan);
  NetDevPtr ethX1 = MakeNetDev(2, "ethX1", DeviceType::Port);
  NetDevPtr ethX2 = MakeNetDev(3, "ethX2", DeviceType::Port);
  NetDevPtr ethX11 = MakeNetDev(4, "ethX11", DeviceType::Port);
  NetDevPtr ethX12 = MakeNetDev(4, "ethX12", DeviceType::Port);

  void SetUp() override {
    mac_assingment_list.clear();
  }

  void CreateDistributorWithMacInc(uint32_t mac_in_inc) {
    mac_distributor_ = make_unique<MacDistributor>(base_mac_, mac_in_inc, mock_mac_controller_);
  }

  void TearDown() override {
  }

  void AssignPortsToBridge(NetDevPtr bridge, initializer_list<NetDevPtr> ports) {
    for (auto port : ports) {
      NetDev::SetRelation(port, bridge);
    }
  }

};

TEST_F(AMacDistributor, AssignsOnlyOneMac) {
  CreateDistributorWithMacInc(1);
  AssignPortsToBridge(br0, { ethX1, ethX2, ethX11, ethX12 });

  netdevs.assign( { br0, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_), _)).Times(netdevs.size());

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsBridgesSameMacs) {

  CreateDistributorWithMacInc(5);

  AssignPortsToBridge(br0, { ethX1, ethX2 });
  AssignPortsToBridge(br1, { ethX11, ethX12 });
  netdevs.assign( { br0, br1, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_), Eq("br0"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_), Eq("br1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(1)), Eq("ethX1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(2)), Eq("ethX2"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(3)), Eq("ethX11"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(4)), Eq("ethX12"))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsBridgesAndPortsDifferentMacs) {

  CreateDistributorWithMacInc(6);

  AssignPortsToBridge(br0, { ethX1, ethX2 });
  AssignPortsToBridge(br1, { ethX11, ethX12 });
  netdevs.assign( { br0, br1, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_), Eq("br0"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(1)), Eq("br1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(2)), Eq("ethX1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(3)), Eq("ethX2"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(4)), Eq("ethX11"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(5)), Eq("ethX12"))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsBridgesWithSinglePorts) {

  CreateDistributorWithMacInc(6);

  AssignPortsToBridge(br0, { ethX1 });
  AssignPortsToBridge(br1, { ethX2 });
  AssignPortsToBridge(br2, { ethX11 });
  AssignPortsToBridge(br3, { ethX12 });
  netdevs.assign( { br0, br1, br2, br3, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_), Eq("br0"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(1)), Eq("br1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(2)), Eq("br2"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(3)), Eq("br3"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(0)), Eq("ethX1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(1)), Eq("ethX2"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(2)), Eq("ethX11"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(3)), Eq("ethX12"))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, DoesNotAssignWwan) {

  CreateDistributorWithMacInc(6);

  AssignPortsToBridge(br0, { ethX1 });
  AssignPortsToBridge(br1, { ethX2 });
  AssignPortsToBridge(br2, { ethX11 });
  AssignPortsToBridge(br3, { ethX12 });
  netdevs.assign( { br0, br1, br2, br3, ethX1, ethX2, ethX11, ethX12, wwan0 });

  EXPECT_CALL(mock_mac_controller_, SetMac(_, _)).Times(8);
  EXPECT_CALL(mock_mac_controller_, SetMac(_, Eq("wwan0"))).Times(0);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsPortsWithoutBridges) {

  CreateDistributorWithMacInc(6);

  netdevs.assign( { ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(0)), Eq("ethX1"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(1)), Eq("ethX2"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(2)), Eq("ethX11"))).Times(1);
  EXPECT_CALL(mock_mac_controller_, SetMac(Eq(base_mac_.Increment(3)), Eq("ethX12"))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}



TEST_F(AMacDistributor, AssignsInitialMacsInInitialSwitchMode) {

  CreateDistributorWithMacInc(6);
  EXPECT_CALL(mock_mac_controller_, SetMac(_, _)).WillRepeatedly(PutToAssignmentList(std::ref(mac_assingment_list)));

  // Assign without br1
  AssignPortsToBridge(br0, { ethX1, ethX2, ethX11, ethX12 });
  netdevs.assign( { br0, ethX1, ethX2, ethX11, ethX12 });

  mac_distributor_->AssignMacs(netdevs);

  EXPECT_EQ(base_mac_.Increment(0), br0->GetMac());

  EXPECT_EQ(5, mac_assingment_list.size());
  EXPECT_THAT(mac_assingment_list, ElementsAre(AssigmentPair { "br0"s, base_mac_.Increment(0) },
                                               AssigmentPair { "ethX1"s, base_mac_.Increment(1) },
                                               AssigmentPair { "ethX2"s, base_mac_.Increment(2) },
                                               AssigmentPair { "ethX11"s, base_mac_.Increment(3) },
                                               AssigmentPair { "ethX12"s, base_mac_.Increment(4) }
                                               ));

}

TEST_F(AMacDistributor, AssignsAlwaysSameMacsToBridges) {

  CreateDistributorWithMacInc(6);
  EXPECT_CALL(mock_mac_controller_, SetMac(_, _)).WillRepeatedly(PutToAssignmentList(std::ref(mac_assingment_list)));

  // Assign without br1
  AssignPortsToBridge(br0, { ethX1, ethX2 });
  AssignPortsToBridge(br2, { ethX11, ethX12 });
  netdevs.assign( { br0, br2, ethX1, ethX2, ethX11, ethX12 });

  mac_distributor_->AssignMacs(netdevs);

  EXPECT_EQ(base_mac_.Increment(0), br0->GetMac());
  EXPECT_EQ(base_mac_.Increment(2), br2->GetMac());
  EXPECT_EQ(6, mac_assingment_list.size());
  EXPECT_THAT(mac_assingment_list, ElementsAre(AssigmentPair { "br0"s, base_mac_.Increment(0) },
                                               AssigmentPair { "br2"s, base_mac_.Increment(2) },
                                               AssigmentPair { "ethX1"s, base_mac_.Increment(1) },
                                               AssigmentPair { "ethX2"s, base_mac_.Increment(3) },
                                               AssigmentPair { "ethX11"s, base_mac_.Increment(4) },
                                               AssigmentPair { "ethX12"s, base_mac_.Increment(5) }
                                               ));

  mac_assingment_list.clear();
  AssignPortsToBridge(br0, { ethX1 });
  AssignPortsToBridge(br1, { ethX2 });
  AssignPortsToBridge(br2, { ethX11, ethX12 });
  netdevs.assign( { br0, br1, br2, ethX1, ethX2, ethX11, ethX12 });
  mac_distributor_->AssignMacs(netdevs);

  EXPECT_EQ(base_mac_.Increment(0), br0->GetMac());
  EXPECT_EQ(base_mac_.Increment(1), br1->GetMac());
  EXPECT_EQ(base_mac_.Increment(2), br2->GetMac());
  EXPECT_EQ(7, mac_assingment_list.size());

  EXPECT_THAT(mac_assingment_list, ElementsAre(AssigmentPair { "br0"s, base_mac_.Increment(0) },
                                               AssigmentPair { "br1"s, base_mac_.Increment(1) },
                                               AssigmentPair { "br2"s, base_mac_.Increment(2) },
                                               AssigmentPair { "ethX1"s, base_mac_.Increment(0) },
                                               AssigmentPair { "ethX2"s, base_mac_.Increment(1) },
                                               AssigmentPair { "ethX11"s, base_mac_.Increment(3) },
                                               AssigmentPair { "ethX12"s, base_mac_.Increment(4) }
                                               ));
}

} /* namespace netconf */
