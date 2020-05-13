// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <functional>
#include <string>
#include <boost/program_options.hpp>

#include "IPConfig.hpp"
#include "IHandler.hpp"
#include "OptionParser.hpp"

namespace network_config
{

  class IPConfigHandler : public IHandler
  {
    public:
      explicit IPConfigHandler(const OptionParser& parser);
      ~IPConfigHandler() override = default;

      IPConfigHandler(const IPConfigHandler&) = delete;
      IPConfigHandler& operator=(const IPConfigHandler&) = delete;
      IPConfigHandler(const IPConfigHandler&&) = delete;
      IPConfigHandler& operator=(const IPConfigHandler&&) = delete;

      void Execute() override;
    private:
      void GetConfig();
      void GetCurrentConfig();
      void ParseConfig(netconf::IPConfigs &ip_configs);
      void SetConfig();
      netconf::IPConfigs CreateIPConfigs();

      ::std::function<void()> execute_;
      const ::boost::program_options::variables_map& vm_;
      const OptionParser& parser_;
  };

} /* namespace network_config */
