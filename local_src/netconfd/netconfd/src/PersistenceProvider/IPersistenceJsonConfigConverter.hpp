// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include "Status.hpp"

#include "Types.hpp"

namespace netconfd {

class IPersistenceJsonConfigConverter {

 public:
  IPersistenceJsonConfigConverter() = default;
  virtual ~IPersistenceJsonConfigConverter() = default;

  IPersistenceJsonConfigConverter(const IPersistenceJsonConfigConverter&) = default;
  IPersistenceJsonConfigConverter& operator=(const IPersistenceJsonConfigConverter&) = default;
  IPersistenceJsonConfigConverter(IPersistenceJsonConfigConverter&&) = default;
  IPersistenceJsonConfigConverter& operator=(IPersistenceJsonConfigConverter&&) = default;

  virtual Status ToJson(const BridgeConfig& bridge_config, const IPConfigs& ip_configs, bool pretty,
                        ::std::string& json) const = 0;

  virtual Status ToConfigs(const ::std::string& json, BridgeConfig& bridge_config, IPConfigs& ip_configs) const = 0;

};

}  // namespace netconfd
