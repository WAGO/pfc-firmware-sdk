// SPDX-License-Identifier: GPL-2.0-or-later

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "PrioritizedHostAndDomainname.hpp"

using namespace testing;

namespace netconf {

class PrioritizedHostAndDomainnameTest : public Test {
 public:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(PrioritizedHostAndDomainnameTest, getPrioritizedHostAndDomainname) {

  PrioritizedHostAndDomainname prio;
  ::std::string prioritized_hostname;
  ::std::string prioritized_domain;

  prio.Add("br1", "host1", "domain1");
  prio.Add("br0", "host0", "domain0");
  prio.Add("br2", "host2", "domain2");

  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);

  EXPECT_EQ("host0", prioritized_hostname);
  EXPECT_EQ("domain0", prioritized_domain);

  prio.Remove("br0");
  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);

  EXPECT_EQ("host1", prioritized_hostname);
  EXPECT_EQ("domain1", prioritized_domain);

}

TEST_F(PrioritizedHostAndDomainnameTest, changePrioritizedHostAndDomainnameEntry) {

  PrioritizedHostAndDomainname prio;

  prio.Add("br1", "host0", "domain0");
  prio.Add("br1", "host1", "domain1");

  ::std::string prioritized_hostname;
  ::std::string prioritized_domain;
  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);

  EXPECT_EQ("host1", prioritized_hostname);
  EXPECT_EQ("domain1", prioritized_domain);
}

TEST_F(PrioritizedHostAndDomainnameTest, getHostAndDomainNameFromTwoDifferentDHCPConfigs) {

  PrioritizedHostAndDomainname prio;
  ::std::string prioritized_hostname;
  ::std::string prioritized_domain;

  prio.Add("br2", "host2", "domain2");
  prio.Add("br0", "host0", "");
  prio.Add("br1", "", "domain1");

  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);

  EXPECT_EQ("host0", prioritized_hostname);
  EXPECT_EQ("domain1", prioritized_domain);

}

TEST_F(PrioritizedHostAndDomainnameTest, wwanInterfaceHasLowerPriorityThanBridges) {

  PrioritizedHostAndDomainname prio;
  ::std::string prioritized_hostname;
  ::std::string prioritized_domain;

  prio.Add("br2", "host2", "domain2");
  prio.Add("wwan0", "host0", "domain0");
  prio.Add("br1", "host1", "domain1");

  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);
  EXPECT_EQ("host1", prioritized_hostname);
  EXPECT_EQ("domain1", prioritized_domain);

  prio.Remove("br1");

  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);
  EXPECT_EQ("host2", prioritized_hostname);
  EXPECT_EQ("domain2", prioritized_domain);

  prio.Remove("br2");

  prio.GetPrioritizedValues(prioritized_hostname, prioritized_domain);
  EXPECT_EQ("host0", prioritized_hostname);
  EXPECT_EQ("domain0", prioritized_domain);
}

TEST_F(PrioritizedHostAndDomainnameTest, getHostname) {
  PrioritizedHostAndDomainname prio;

  prio.Add("br2", "host2", "domain2");
  prio.Add("br0", "host0", "");
  prio.Add("br1", "", "domain1");

  EXPECT_EQ("host0", prio.GetHostname("br0"));
  EXPECT_EQ("", prio.GetHostname("br1"));
  EXPECT_EQ("host2", prio.GetHostname("br2"));

}

}  // namespace netconf
