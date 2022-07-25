// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>
#include "DataFieldAccess.hpp"

namespace network_config {

class InterfaceStatusHandler : public IHandler {
 public:
  explicit InterfaceStatusHandler(const ::boost::program_options::variables_map &vm);
  ~InterfaceStatusHandler() override = default;

  InterfaceStatusHandler(const InterfaceStatusHandler&) = delete;
  InterfaceStatusHandler& operator=(const InterfaceStatusHandler&) = delete;
  InterfaceStatusHandler(const InterfaceStatusHandler&&) = delete;
  InterfaceStatusHandler& operator=(const InterfaceStatusHandler&&) = delete;

  void Execute() override;

 private:
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
