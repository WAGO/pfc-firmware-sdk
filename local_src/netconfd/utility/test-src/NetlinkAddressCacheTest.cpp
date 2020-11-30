// SPDX-License-Identifier: LGPL-3.0-or-later
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "NetlinkAddressCache.hpp"
#include "NetlinkMonitor.hpp"

using namespace testing;

namespace netconf {
class NetlinkAddressCacheTest_Target : public Test {
public:
    NetlinkMonitor monitor_;
    ::std::shared_ptr<NetlinkAddressCache> out_;

    NetlinkAddressCacheTest_Target() {
        out_ = monitor_.Add<NetlinkAddressCache>();
    }
};

TEST_F(NetlinkAddressCacheTest_Target, GetIPAddress) {
    auto ip = out_->GetIPAddress(1);
    ASSERT_EQ("127.0.0.1", ip);

    ip = out_->GetIPAddress(7);
    ASSERT_EQ("172.29.234.12", ip);
}

}  // namespace netconf
