// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include "NetDev.hpp"

using namespace testing;

namespace netconf {
class NetDevRelationsTest : public Test {
 public:
};

TEST_F(NetDevRelationsTest, NewRelation) {

  NetDevPtr parent = MakeNetDev(0,"parent","parent", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr child = MakeNetDev(0,"child","child", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);


  NetDev::SetRelation(child, parent);

  EXPECT_EQ(1, parent->GetChildren().size());
  EXPECT_EQ(child, parent->GetChildren().front());

}

TEST_F(NetDevRelationsTest, AppendRelation) {

  NetDevPtr parent = MakeNetDev(0,"parent","parent", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr child1 = MakeNetDev(0,"child1","child1", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child1, parent);

  NetDevPtr child2 = MakeNetDev(0,"child2","child2", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child2, parent);

  EXPECT_EQ(2, parent->GetChildren().size());
  EXPECT_EQ(child1, *std::find(parent->GetChildren().begin(), parent->GetChildren().end(), child1));
  EXPECT_EQ(child2, *std::find(parent->GetChildren().begin(), parent->GetChildren().end(), child2));

}

TEST_F(NetDevRelationsTest, MoveRelation) {

  NetDevPtr parent1 = MakeNetDev(0,"parent1","parent1", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr child = MakeNetDev(0,"child1","child1", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child, parent1);

  NetDevPtr parent2 = MakeNetDev(0,"parent2","parent2", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child, parent2);


  EXPECT_EQ(0, parent1->GetChildren().size());
  EXPECT_FALSE(parent1->GetParent());

  EXPECT_EQ(1, parent2->GetChildren().size());
  EXPECT_EQ(child, parent2->GetChildren().front());
  EXPECT_FALSE(parent2->GetParent());

  EXPECT_EQ( parent2, child->GetParent());
  EXPECT_EQ(0, child->GetChildren().size());
}

TEST_F(NetDevRelationsTest, RemoveParentRelation) {

  NetDevPtr parent = MakeNetDev(0,"parent","parent", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr child = MakeNetDev(0,"child","child", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child, parent);

  NetDev::RemoveRelation(child, parent);

  EXPECT_FALSE(child->GetParent());
  EXPECT_EQ(0u, parent->GetChildren().size());
}

TEST_F(NetDevRelationsTest, RemoveOnlyWhenRelated) {

  NetDevPtr parent = MakeNetDev(0,"parent","parent", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr parent2 = MakeNetDev(0,"parent2","parent2", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr child = MakeNetDev(0,"child","child", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);
  NetDevPtr child2 = MakeNetDev(0,"child2","child2", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child, parent);
  NetDev::SetRelation(child2, parent2);

  NetDev::RemoveRelation(child2, parent);
  EXPECT_EQ(parent, child->GetParent());
  EXPECT_EQ(child, *std::find(parent->GetChildren().begin(), parent->GetChildren().end(), child));


  NetDev::RemoveRelation(child, parent2);
  EXPECT_EQ(parent, child->GetParent());
  EXPECT_EQ(child, *std::find(parent->GetChildren().begin(), parent->GetChildren().end(), child));


  auto netdev = NetDevPtr{};
  NetDev::RemoveRelation(netdev, parent);
  EXPECT_EQ(parent, child->GetParent());
  EXPECT_EQ(child, *std::find(parent->GetChildren().begin(), parent->GetChildren().end(), child));

  NetDev::RemoveRelation(child, netdev);
  EXPECT_EQ(parent, child->GetParent());
  EXPECT_EQ(child, *std::find(parent->GetChildren().begin(), parent->GetChildren().end(), child));



}

TEST_F(NetDevRelationsTest, NoDuplicateRelations) {

  NetDevPtr parent = MakeNetDev(0,"parent","parent", DeviceType::Bridge, true, eth::InterfaceLinkState::Down);
  NetDevPtr child = MakeNetDev(0,"child","child", DeviceType::Ethernet, false, eth::InterfaceLinkState::Down);

  NetDev::SetRelation(child, parent);
  NetDev::SetRelation(child, parent);

  EXPECT_EQ(parent, child->GetParent());
  EXPECT_EQ(1u, parent->GetChildren().size());
}


}  // namespace netconfd

