// SPDX-License-Identifier: GPL-2.0-or-later

#include "ReloadHostConfHandler.hpp"

#include "NetconfStatus.hpp"

#include <string>
#include <exception>
#include <Event.hpp>

#include "Utils.hpp"

namespace network_config {

namespace napi = ::netconf::api;

void ReloadHostConfHandler::Execute() {
  auto error = netconf::api::ReloadHostConf();
    if (error.IsNotOk()) {
      throw NetconfStatus(error);
    }
}

}
