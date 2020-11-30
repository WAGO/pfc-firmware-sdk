// SPDX-License-Identifier: GPL-2.0-or-later

#include "OutputFactory.hpp"
#include "OptionStrings.hpp"
#include "Utils.hpp"

namespace network_config {

::std::unique_ptr<Output> OutputFactory::Create(const po::variables_map &vm) {
  const auto &optstr = GetOptions();
  if (Contains(vm, optstr.quiet)) {
    return ::std::make_unique<NullOutput>();
  } else if (Contains(vm, optstr.error_msg_dst)) {
    return ::std::make_unique < FileOutput > (Get(vm, optstr.error_msg_dst));
  } else {
    return ::std::make_unique<StderrOutput>();
  }
}

}  // namespace network_config
