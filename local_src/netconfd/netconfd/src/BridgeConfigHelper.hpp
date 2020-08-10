//------------------------------------------------------------------------------
#ifndef NETCONFD_SRC_BRIDGECONFIGHELPER_HPP_
#define NETCONFD_SRC_BRIDGECONFIGHELPER_HPP_

#include <initializer_list>
#include "Types.hpp"

namespace netconf {

bool AreInSameBridge(const BridgeConfig& bc, const Interfaces& interfaces);

}  // namespace netonfd

#endif /* NETCONFD_SRC_BRIDGECONFIGHELPER_HPP_ */

