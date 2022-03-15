// SPDX-License-Identifier: GPL-2.0-or-later
#include "BridgeConfigHelper.hpp"
#include "CollectionUtils.hpp"

#include <algorithm>

namespace netconf {

bool AreInSameBridge(const BridgeConfig& bc, const Interfaces& interfaces) {

  BridgeConfig bridges;

  for (auto &itf : interfaces) {
    auto it = ::std::find_if(bc.begin(), bc.end(), [&](auto &map_pair) { return IsIncluded(itf, ::std::get<1>(map_pair)); });
    if(it == bc.end()){
      return false;
    }
  }

  auto inserter = std::inserter(bridges, bridges.begin());
  for (auto &itf : interfaces) {
    ::std::copy_if(bc.begin(), bc.end(), inserter, [&](auto &map_pair) {
      return IsIncluded(itf, ::std::get<1>(map_pair));
    });
  }

  return bridges.size() == 1;
}



}  // namespace netconf
