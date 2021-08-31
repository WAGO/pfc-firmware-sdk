// SPDX-License-Identifier: GPL-2.0-or-later

#include "FixIpHandler.hpp"

#include <string>

#include "IPConfig.hpp"
#include "NetconfStatus.hpp"

namespace network_config {
void FixIpHandler::Execute() {
  auto error = netconf::api::SetTempFixIp();
  if (error.IsNotOk()) {
    throw NetconfStatus(error);
  }
}

} /* namespace network_config */
