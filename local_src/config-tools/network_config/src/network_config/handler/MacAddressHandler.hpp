// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

namespace network_config {

class MacAddressHandler : public IHandler {
 public:
  explicit MacAddressHandler(const ::boost::program_options::variables_map &vm);
  ~MacAddressHandler() override = default;

  MacAddressHandler(const MacAddressHandler&) = delete;
  MacAddressHandler& operator=(const MacAddressHandler&) = delete;
  MacAddressHandler(const MacAddressHandler&&) = delete;
  MacAddressHandler& operator=(const MacAddressHandler&&) = delete;

  void Execute() override;

 private:
  void GetMacAddress();

  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
