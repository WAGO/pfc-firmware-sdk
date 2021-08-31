// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

namespace network_config {

class DSAModeHandler : public IHandler {
 public:
  explicit DSAModeHandler(const ::boost::program_options::variables_map &vm);
  ~DSAModeHandler() override = default;

  DSAModeHandler(const DSAModeHandler&) = delete;
  DSAModeHandler& operator=(const DSAModeHandler&) = delete;
  DSAModeHandler(const DSAModeHandler&&) = delete;
  DSAModeHandler& operator=(const DSAModeHandler&&) = delete;

  void Execute() override;
 private:
  void GetConfig();
  void SetConfig();

  ::std::function<void()> execute_;
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
