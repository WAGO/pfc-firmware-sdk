// SPDX-License-Identifier: GPL-2.0-or-later

#include "FixIpHandler.hpp"
#include "IPConfig.hpp"

#include <string>

namespace network_config
{
  FixIpHandler::FixIpHandler()
  {
      execute_ = [this]() { this->SetConfig();};
  }

  void FixIpHandler::Execute()
  {
    execute_();
  }

  void FixIpHandler::SetConfig()
  {
  netconf::SetTempFixIp();
  }

} /* namespace network_config */
