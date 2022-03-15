// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
#include "BridgeConfigHelper.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "CollectionUtils.hpp"
#include "boost/algorithm/string/join.hpp"

using namespace testing;

namespace netconf {

struct TestTuple {
  Interfaces interfaces;
  bool expectation;

  ::std::ostream& operator<<(::std::ostream& os){
    os << "Interfaces" << "{" << ::boost::algorithm::join(interfaces, ",") << "}\n";
    os << "Expected" << expectation << "\n";
    return os;
  }
};

class BridgeConfigHelperTest : public Test, public WithParamInterface<TestTuple> {
 public:

  BridgeConfig bc { { "br0", { "X1", "X2" } }, { "br1", { "X11", "X42", "X1337" } }, { "br2", { "X12" } },
      { "br3", { } } };

};

TEST_P(BridgeConfigHelperTest, AreInSameBridge) {

  const auto &tested_interfaces = GetParam().interfaces;

  EXPECT_EQ(GetParam().expectation, AreInSameBridge(bc, tested_interfaces));

}

INSTANTIATE_TEST_CASE_P(Instances, BridgeConfigHelperTest,
                        testing::Values(
                            TestTuple { { "X1", "X2" }, true },
                            TestTuple { { "X11", "X1337" }, true },
                            TestTuple { { "X42", "X1" }, false },
                            TestTuple { { "X999"}, false },
                            TestTuple { { "X1", "X999"}, false },
                            TestTuple { { "X12" }, true },
                            TestTuple { { }, false }
                        ));

}
