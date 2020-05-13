// SPDX-License-Identifier: GPL-2.0-or-later

#include "OptionStrings.hpp"

namespace network_config {

static const Options options;
static const Fields fields;

[[gnu::const]]
const Options& GetOptions() {
  return options;
}

[[gnu::const]]
const Fields& GetFields() {
  return fields;
}

}  // namespace network_config
