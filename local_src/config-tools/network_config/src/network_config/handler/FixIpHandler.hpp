// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>


namespace network_config
{

  class FixIpHandler : public IHandler {
    public:
      explicit FixIpHandler();
      ~FixIpHandler() override = default;

      FixIpHandler(const FixIpHandler&) = delete;
      FixIpHandler& operator=(const FixIpHandler&) = delete;
      FixIpHandler(const FixIpHandler&&) = delete;
      FixIpHandler& operator=(const FixIpHandler&&) = delete;

      void Execute() override;
    private:
      void SetConfig();

      ::std::function<void()> execute_;
  };

} /* namespace network_config */
