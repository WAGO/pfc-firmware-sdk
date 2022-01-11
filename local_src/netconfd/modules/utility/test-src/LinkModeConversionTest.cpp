#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <linux/ethtool.h>
#include "LinkModeConversion.hpp"

namespace netconf {

using namespace testing;
/*
 *  ETHTOOL_LINK_MODE_10baseT_Half_BIT  = 0,
 ETHTOOL_LINK_MODE_10baseT_Full_BIT  = 1,
 ETHTOOL_LINK_MODE_100baseT_Half_BIT = 2,
 ETHTOOL_LINK_MODE_100baseT_Full_BIT = 3,
 ETHTOOL_LINK_MODE_1000baseT_Half_BIT  = 4,
 ETHTOOL_LINK_MODE_1000baseT_Full_BIT  = 5,
 ETHTOOL_LINK_MODE_Autoneg_BIT   = 6,
 ETHTOOL_LINK_MODE_TP_BIT    = 7,
 ETHTOOL_LINK_MODE_AUI_BIT   = 8,
 ETHTOOL_LINK_MODE_MII_BIT   = 9,
 ETHTOOL_LINK_MODE_FIBRE_BIT   = 10,
 ETHTOOL_LINK_MODE_BNC_BIT   = 11,
 ETHTOOL_LINK_MODE_10000baseT_Full_BIT = 12,
 ETHTOOL_LINK_MODE_Pause_BIT   = 13,
 ETHTOOL_LINK_MODE_Asym_Pause_BIT  = 14,
 ETHTOOL_LINK_MODE_2500baseX_Full_BIT  = 15,
 ETHTOOL_LINK_MODE_Backplane_BIT   = 16,
 ETHTOOL_LINK_MODE_1000baseKX_Full_BIT = 17,
 ETHTOOL_LINK_MODE_10000baseKX4_Full_BIT = 18,
 ETHTOOL_LINK_MODE_10000baseKR_Full_BIT  = 19,
 ETHTOOL_LINK_MODE_10000baseR_FEC_BIT  = 20,
 ETHTOOL_LINK_MODE_20000baseMLD2_Full_BIT = 21,
 ETHTOOL_LINK_MODE_20000baseKR2_Full_BIT = 22,
 ETHTOOL_LINK_MODE_40000baseKR4_Full_BIT = 23,
 ETHTOOL_LINK_MODE_40000baseCR4_Full_BIT = 24,
 ETHTOOL_LINK_MODE_40000baseSR4_Full_BIT = 25,
 ETHTOOL_LINK_MODE_40000baseLR4_Full_BIT = 26,
 ETHTOOL_LINK_MODE_56000baseKR4_Full_BIT = 27,
 ETHTOOL_LINK_MODE_56000baseCR4_Full_BIT = 28,
 ETHTOOL_LINK_MODE_56000baseSR4_Full_BIT = 29,
 ETHTOOL_LINK_MODE_56000baseLR4_Full_BIT = 30,
 ETHTOOL_LINK_MODE_25000baseCR_Full_BIT  = 31,
 ETHTOOL_LINK_MODE_25000baseKR_Full_BIT  = 32,
 ETHTOOL_LINK_MODE_25000baseSR_Full_BIT  = 33,
 ETHTOOL_LINK_MODE_50000baseCR2_Full_BIT = 34,
 ETHTOOL_LINK_MODE_50000baseKR2_Full_BIT = 35,
 ETHTOOL_LINK_MODE_100000baseKR4_Full_BIT  = 36,
 ETHTOOL_LINK_MODE_100000baseSR4_Full_BIT  = 37,
 ETHTOOL_LINK_MODE_100000baseCR4_Full_BIT  = 38,
 ETHTOOL_LINK_MODE_100000baseLR4_ER4_Full_BIT  = 39,
 ETHTOOL_LINK_MODE_50000baseSR2_Full_BIT   = 40,
 ETHTOOL_LINK_MODE_1000baseX_Full_BIT  = 41,
 ETHTOOL_LINK_MODE_10000baseCR_Full_BIT  = 42,
 ETHTOOL_LINK_MODE_10000baseSR_Full_BIT  = 43,
 ETHTOOL_LINK_MODE_10000baseLR_Full_BIT  = 44,
 ETHTOOL_LINK_MODE_10000baseLRM_Full_BIT = 45,
 ETHTOOL_LINK_MODE_10000baseER_Full_BIT  = 46,
 ETHTOOL_LINK_MODE_2500baseT_Full_BIT  = 47,
 ETHTOOL_LINK_MODE_5000baseT_Full_BIT  = 48,

 ETHTOOL_LINK_MODE_FEC_NONE_BIT  = 49,
 ETHTOOL_LINK_MODE_FEC_RS_BIT  = 50,
 ETHTOOL_LINK_MODE_FEC_BASER_BIT = 51,
 */


template<size_t N, typename ... Args>
constexpr auto generateLinkModes(Args ...args) {
  const std::array<uint32_t, sizeof...(Args)> bits {{(args)...}};
  std::array<uint32_t, N> modes{};
  for(auto bit : bits){
    modes[bit/32] |= 1 << (bit % 32);
  }
  return modes;
}

TEST(LinkModeConversionTest, ConvertToLinkModeVector) {

  auto ethtool_link_modes = generateLinkModes<4>(ETHTOOL_LINK_MODE_10baseT_Half_BIT, ETHTOOL_LINK_MODE_10baseT_Full_BIT,
                                                 ETHTOOL_LINK_MODE_100baseT_Half_BIT, ETHTOOL_LINK_MODE_100baseT_Full_BIT, ETHTOOL_LINK_MODE_10000baseER_Full_BIT);
  auto linkmodes = ToLinkModes(ethtool_link_modes);


  EXPECT_THAT(linkmodes, ElementsAre(LinkMode{10, Duplex::HALF}, LinkMode{10, Duplex::FULL}, LinkMode{100, Duplex::HALF}, LinkMode{100, Duplex::FULL}, LinkMode{10000, Duplex::FULL}));

}

}  // namespace netconfd
