// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <boost/program_options.hpp>
#include <functional>
#include <string>

namespace network_config {

class DipSwitchHandler : public IHandler {
 public:
  explicit DipSwitchHandler(const ::boost::program_options::variables_map &vm);
  ~DipSwitchHandler() override = default;

  DipSwitchHandler(const DipSwitchHandler &) = delete;
  DipSwitchHandler &operator=(const DipSwitchHandler &) = delete;
  DipSwitchHandler(const DipSwitchHandler &&)           = delete;
  DipSwitchHandler &operator=(const DipSwitchHandler &&) = delete;

  void Execute() override;

 private:
  bool Is(const ::std::string &command);
  ::std::string Get(const ::std::string &command);
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
