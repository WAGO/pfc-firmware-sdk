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
    using TypeFilter = boost::optional<netconf::DeviceType>;
      explicit IPConfigHandler(const OptionParser& parser);
      ~IPConfigHandler() = default;

      IPConfigHandler(const IPConfigHandler&) = delete;
      IPConfigHandler& operator=(const IPConfigHandler&) = delete;
      IPConfigHandler(IPConfigHandler&&);
      IPConfigHandler& operator=(IPConfigHandler&&) = delete;

      void Execute() override;
    private:
      void GetConfig(TypeFilter filter);
      void GetCurrentConfig(TypeFilter filter);
      void ParseConfig(netconf::api::IPConfigs &ip_configs);
      void SetConfig();
      netconf::api::IPConfigs CreateIPConfigs();

      ::std::function<void()> execute_;
      const ::boost::program_options::variables_map& vm_;
      const OptionParser& parser_;
  };

} /* namespace network_config */
