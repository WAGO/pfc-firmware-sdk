// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IPLink.hpp"
#include "MockIEventManager.hpp"
#include "MockIIPConfigure.hpp"
#include "MockIGratuitousArp.hpp"
#include "MockIIPLinks.hpp"
#include "NetDev.hpp"

using namespace testing;

namespace netconf {

class IpLinkTest : public Test {
 public:
  NiceMock<MockIIPConfigure> mock_ip_configure_;
  MockIEventManager mock_event_manager_;
  MockIGratuitousArp mock_gratuitous_arp_;
  MockIIPLinks mock_ip_link_;
  NetDevPtr netdev;

  ::std::unique_ptr<IPLink> sut_;
  void SetUp() override {
    netdev = std::make_shared<NetDev>(0, "br0", DeviceType::Bridge);
    netdev->SetIfFlags(1 << 16);
    sut_ = ::std::make_unique<IPLink>(mock_ip_configure_, mock_event_manager_, mock_gratuitous_arp_, mock_ip_link_, netdev, "", "");
  }

  void TearDown() override {

  }

};

class IpLinkFlushTest : public IpLinkTest, public testing::WithParamInterface<IPSource> {
 public:

};

MATCHER_P(IpConfigSourceEq, source, "To match the source"){
return arg.source_ == source;
}

TEST_F(IpLinkTest, FlushOnDestruction) {

  EXPECT_CALL(mock_ip_configure_, Configure(IpConfigSourceEq(IPSource::NONE))).WillOnce(Return(Status { }));

  {
    IPLink link(mock_ip_configure_, mock_event_manager_, mock_gratuitous_arp_, mock_ip_link_, netdev, "", "");
  }
  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}

TEST_F(IpLinkTest, NewSourceStatic) {

  EXPECT_CALL(mock_ip_configure_, Configure(IpConfigSourceEq(IPSource::STATIC))).WillOnce(Return(Status { }));
  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(1);

  sut_->SetIPConfig( { "br0", IPSource::STATIC });

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}

TEST_F(IpLinkTest, SetSameIpNoNotify) {

  EXPECT_CALL(mock_ip_configure_, Configure(IpConfigSourceEq(IPSource::STATIC))).Times(2).WillRepeatedly(Return(Status { }));
  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(1);

  sut_->SetIPConfig( { "br0", IPSource::STATIC });
  sut_->SetIPConfig( { "br0", IPSource::STATIC });

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}

TEST_F(IpLinkTest, ChangeOfSourcesNotifies) {

  EXPECT_CALL(mock_ip_configure_, Configure(_)).WillRepeatedly(Return(Status { }));
  auto config = IPConfig{ "br0", IPSource::STATIC, "192.168.42.42",  "255.255.255.0"};
  sut_->SetIPConfig( config );

  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(3);
  config.source_ = IPSource::BOOTP;
  sut_->SetIPConfig( config );
  config.source_ = IPSource::DHCP;
  sut_->SetIPConfig( config );
  config.source_ = IPSource::STATIC;
  sut_->SetIPConfig( config );

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}

TEST_F(IpLinkTest, SameServiceSourceNoTrigger) {
  /* When source is still DHCP but the address changes the event folder is not triggered */
  EXPECT_CALL(mock_ip_configure_, Configure(_)).WillRepeatedly(Return(Status { }));
  auto config = IPConfig{ "br0", IPSource::DHCP, "192.168.42.42",  "255.255.255.0"};
  sut_->SetIPConfig( config );

  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(0);
  config.address_ = "192.168.13.37";
  sut_->SetIPConfig( config );
  config.netmask_ = "255.0.0.0";
  sut_->SetIPConfig( config );

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}
TEST_F(IpLinkTest, SetSameIpWithTemporaryDoesNotify) {

  EXPECT_CALL(mock_ip_configure_, Configure(_)).WillRepeatedly(Return(Status { }));
  sut_->SetIPConfig( { "br0", IPSource::DHCP });

  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(2);

  sut_->SetIPConfig( { "br0", IPSource::TEMPORARY });
  sut_->SetIPConfig( { "br0", IPSource::TEMPORARY });

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}

TEST_F(IpLinkTest, SetDifferentIpWithSourceTemporaryNotifies) {

  EXPECT_CALL(mock_ip_configure_, Configure(_)).WillRepeatedly(Return(Status { }));
  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(1);
  sut_->SetIPConfig( { "br0", IPSource::DHCP });

  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(3);

  sut_->SetIPConfig( { "br0", IPSource::TEMPORARY, "192.168.3.3", "255.255.255.0"});
  sut_->SetIPConfig( { "br0", IPSource::TEMPORARY, "192.168.4.3", "255.255.255.0"});
  sut_->SetIPConfig( { "br0", IPSource::TEMPORARY, "192.168.4.3", "255.255.0.0"});

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);

}

TEST_P(IpLinkFlushTest, NewSourceDhcpFlushesFirst) {

  auto new_source = GetParam();

  // First the flush
  EXPECT_CALL(mock_ip_configure_, Configure(IpConfigSourceEq(IPSource::NONE))).WillOnce(Return(Status { }));
  // The the set of the new parameter
  EXPECT_CALL(mock_ip_configure_, Configure(IpConfigSourceEq(new_source))).WillOnce(Return(Status { }));
  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(_,_,_)).Times(1);

  sut_->SetIPConfig( { "br0", new_source });

  Mock::VerifyAndClearExpectations(&mock_ip_configure_);
}
INSTANTIATE_TEST_CASE_P(IpLinkFlushingTest, IpLinkFlushTest,
                        testing::Values(IPSource::DHCP, IPSource::BOOTP, IPSource::EXTERNAL));

}
