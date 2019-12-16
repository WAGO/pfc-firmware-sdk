// SPDX-License-Identifier: GPL-2.0-or-later


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Types.hpp"
#include "MockIDevicePropertiesProvider.hpp"
#include <memory>
#include "BridgeConfigTransformator.hpp"

using namespace testing;

namespace netconfd {

class ABridgeConfigTransformator : public ::testing::Test {
 public:

  MockIDevicePropertiesProvider mock_properties_provider_;
  ::std::shared_ptr<BridgeConfigTransformator> transformator_;

  ABridgeConfigTransformator() {

    InterfaceNameMapping interface_name_mapping = { { "X1", "ethX1" }, { "X2", "ethX2" },
        { "X11", "ethX11" }, { "X12", "ethX12" } };

    EXPECT_CALL(mock_properties_provider_, GetInterfacesNameMapping()).WillRepeatedly(
        Return(interface_name_mapping));

    transformator_ = ::std::make_shared<BridgeConfigTransformator>(
        mock_properties_provider_);
  }

  void SetUp() override {

  }

  void TearDown() override {

  }

};

TEST_F(ABridgeConfigTransformator, TransformsAnInterfaceOfOneBridgeToOS) {
  BridgeConfig config = { { "br0", { "X1" } } };
  BridgeConfig config_os = transformator_->ConvertProductToOS(config);

  EXPECT_EQ(config_os["br0"][0], "ethX1");
}

TEST_F(ABridgeConfigTransformator, TransformsThreeInterfaceOfOneBridgeToOS) {
  BridgeConfig config = { { "br0", { "X12", "X2", "X1" } } };
  BridgeConfig config_os = transformator_->ConvertProductToOS(config);

  EXPECT_EQ(config_os["br0"][0], "ethX12");
  EXPECT_EQ(config_os["br0"][1], "ethX2");
  EXPECT_EQ(config_os["br0"][2], "ethX1");
}

TEST_F(ABridgeConfigTransformator, TransformsInterfacesOfTwoBridgesToOS) {
  BridgeConfig config = { { "br0", { "X12", "X2" } }, { "br1", { "X1", "X11" } } };
  BridgeConfig config_os = transformator_->ConvertProductToOS(config);

  EXPECT_EQ(config_os["br0"][0], "ethX12");
  EXPECT_EQ(config_os["br0"][1], "ethX2");
  EXPECT_EQ(config_os["br1"][0], "ethX1");
  EXPECT_EQ(config_os["br1"][1], "ethX11");
}

TEST_F(ABridgeConfigTransformator, TransformsNotMappedInterfaceToOS) {
  BridgeConfig config = { { "br0", { "X12", "R2" } }, { "br1", { "D2", "X11" } } };
  BridgeConfig config_os = transformator_->ConvertProductToOS(config);

  EXPECT_EQ(config_os["br0"][0], "ethX12");
  EXPECT_EQ(config_os["br0"][1], "R2");
  EXPECT_EQ(config_os["br1"][0], "D2");
  EXPECT_EQ(config_os["br1"][1], "ethX11");
}

TEST_F(ABridgeConfigTransformator, TransformsAnInterfaceOfOneBridgeToProduct) {
  BridgeConfig config = { { "br0", { "ethX1" } } };
  BridgeConfig config_product = transformator_->ConvertOSToProduct(config);

  EXPECT_EQ(config_product["br0"][0], "X1");
}

TEST_F(ABridgeConfigTransformator, TransformsThreeInterfaceOfOneBridgeToProduct) {
  BridgeConfig config = { { "br0", { "ethX12", "ethX2", "ethX1" } } };
  BridgeConfig config_product = transformator_->ConvertOSToProduct(config);

  EXPECT_EQ(config_product["br0"][0], "X12");
  EXPECT_EQ(config_product["br0"][1], "X2");
  EXPECT_EQ(config_product["br0"][2], "X1");
}

TEST_F(ABridgeConfigTransformator, TransformsInterfacesOfTwoBridgesToProduct) {
  BridgeConfig config = { { "br0", { "ethX12", "ethX2" } },
      { "br1", { "ethX1", "ethX11" } } };
  BridgeConfig config_product = transformator_->ConvertOSToProduct(config);

  EXPECT_EQ(config_product["br0"][0], "X12");
  EXPECT_EQ(config_product["br0"][1], "X2");
  EXPECT_EQ(config_product["br1"][0], "X1");
  EXPECT_EQ(config_product["br1"][1], "X11");
}

TEST_F(ABridgeConfigTransformator, TransformsNotMappedInterfaceToProduct) {
  BridgeConfig config = { { "br0", { "ethX12", "R2" } }, { "br1", { "D2", "ethX11" } } };
  BridgeConfig config_product = transformator_->ConvertOSToProduct(config);

  EXPECT_EQ(config_product["br0"][0], "X12");
  EXPECT_EQ(config_product["br0"][1], "R2");
  EXPECT_EQ(config_product["br1"][0], "D2");
  EXPECT_EQ(config_product["br1"][1], "X11");

}

}  // namespace netconfd
//---- End of source file ------------------------------------------------------
