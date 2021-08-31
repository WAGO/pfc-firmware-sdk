// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IHandler.hpp"

#include <boost/program_options.hpp>
#include <memory>

#include "OptionParser.hpp"

namespace network_config {

class HandlerFactory {
 public:
  static ::std::unique_ptr<IHandler> CreateHandler(const OptionParser &parser);
};

}
