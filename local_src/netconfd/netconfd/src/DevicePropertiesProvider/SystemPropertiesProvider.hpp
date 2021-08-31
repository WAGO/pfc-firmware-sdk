// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

#include "Status.hpp"

namespace netconf {

class SystemPropertiesProvider {
 public:
  SystemPropertiesProvider() = default;
  virtual ~SystemPropertiesProvider() = default;

  SystemPropertiesProvider(const SystemPropertiesProvider&) = delete;
  SystemPropertiesProvider& operator=(const SystemPropertiesProvider&) = delete;
  SystemPropertiesProvider(SystemPropertiesProvider&&) = delete;
  SystemPropertiesProvider& operator=(SystemPropertiesProvider&&) = delete;

  ::std::string GetHostname() const;
};

}
