// SPDX-License-Identifier: LGPL-3.0-or-later

#include "LinkModeConversion.hpp"

#include <linux/ethtool.h>

namespace netconf {

namespace {
LinkMode ToLinkMode(ethtool_link_mode_bit_indices index) {
  switch (index) {
      case ETHTOOL_LINK_MODE_10baseT_Half_BIT:
        return LinkMode{10, Duplex::HALF};

      case ETHTOOL_LINK_MODE_10baseT_Full_BIT:
        return LinkMode{10, Duplex::FULL};

      case ETHTOOL_LINK_MODE_100baseT_Half_BIT:
        return LinkMode{100, Duplex::HALF};

      case ETHTOOL_LINK_MODE_100baseT_Full_BIT:
        return LinkMode{100, Duplex::FULL};

      case ETHTOOL_LINK_MODE_1000baseT_Half_BIT:
        return LinkMode{1000, Duplex::HALF};

      case ETHTOOL_LINK_MODE_1000baseX_Full_BIT:
      case ETHTOOL_LINK_MODE_1000baseKX_Full_BIT:
      case ETHTOOL_LINK_MODE_1000baseT_Full_BIT:
        return LinkMode{1000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_2500baseX_Full_BIT:
      case ETHTOOL_LINK_MODE_2500baseT_Full_BIT:
        return LinkMode{2500, Duplex::FULL};

      case ETHTOOL_LINK_MODE_5000baseT_Full_BIT:
        return LinkMode{5000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_10000baseT_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseKX4_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseKR_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseR_FEC_BIT:
      case ETHTOOL_LINK_MODE_10000baseCR_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseSR_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseLR_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseLRM_Full_BIT:
      case ETHTOOL_LINK_MODE_10000baseER_Full_BIT:
        return LinkMode{10000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_20000baseMLD2_Full_BIT:
      case ETHTOOL_LINK_MODE_20000baseKR2_Full_BIT:
        return LinkMode{20000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_40000baseKR4_Full_BIT:
      case ETHTOOL_LINK_MODE_40000baseCR4_Full_BIT:
      case ETHTOOL_LINK_MODE_40000baseSR4_Full_BIT:
      case ETHTOOL_LINK_MODE_40000baseLR4_Full_BIT:
        return LinkMode{40000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_56000baseKR4_Full_BIT:
      case ETHTOOL_LINK_MODE_56000baseCR4_Full_BIT:
      case ETHTOOL_LINK_MODE_56000baseSR4_Full_BIT:
      case ETHTOOL_LINK_MODE_56000baseLR4_Full_BIT:
        return LinkMode{56000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_25000baseCR_Full_BIT:
      case ETHTOOL_LINK_MODE_25000baseKR_Full_BIT:
      case ETHTOOL_LINK_MODE_25000baseSR_Full_BIT:
        return LinkMode{25000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_50000baseCR2_Full_BIT:
      case ETHTOOL_LINK_MODE_50000baseKR2_Full_BIT:
      case ETHTOOL_LINK_MODE_50000baseSR2_Full_BIT :
        return LinkMode{50000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_100000baseKR4_Full_BIT:
      case ETHTOOL_LINK_MODE_100000baseSR4_Full_BIT:
      case ETHTOOL_LINK_MODE_100000baseCR4_Full_BIT:
      case ETHTOOL_LINK_MODE_100000baseLR4_ER4_Full_BIT:
        return LinkMode{100000, Duplex::FULL};

      case ETHTOOL_LINK_MODE_Backplane_BIT :
      case ETHTOOL_LINK_MODE_FEC_NONE_BIT:
      case ETHTOOL_LINK_MODE_FEC_RS_BIT:
      case ETHTOOL_LINK_MODE_FEC_BASER_BIT:
      case ETHTOOL_LINK_MODE_Autoneg_BIT :
      case ETHTOOL_LINK_MODE_TP_BIT  :
      case ETHTOOL_LINK_MODE_AUI_BIT :
      case ETHTOOL_LINK_MODE_MII_BIT :
      case ETHTOOL_LINK_MODE_FIBRE_BIT :
      case ETHTOOL_LINK_MODE_BNC_BIT :
      case ETHTOOL_LINK_MODE_Pause_BIT :
      case ETHTOOL_LINK_MODE_Asym_Pause_BIT:
        return LinkMode{};

    default:
      return LinkMode{};
  }
}
}

constexpr bool BitIsSet(::std::uint32_t value, ::std::uint32_t bit) {
    return (value & (1u << bit)) == (1u<<bit);
}

LinkModes ToLinkModes(gsl::span<const uint32_t> linkmodes) {
  LinkModes modes;
  auto bitIndex = 0;

  for(auto linkmode: linkmodes) {
    for(auto i = 0U; i < 32; ++i) {
      if (BitIsSet(linkmode, i)) {
        auto lm = ToLinkMode(static_cast<ethtool_link_mode_bit_indices>(bitIndex));
        if (lm)
          modes.push_back(lm);
      }
      bitIndex++;
    }
  }

  return modes;
}


} /* namespace netconf */
