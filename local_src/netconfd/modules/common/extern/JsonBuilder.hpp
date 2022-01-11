// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "JsonConverter.hpp"
#include "FirmwareVersion.hpp"

namespace netconf {

class JsonBuilder {

 public:
  template<class T> JsonBuilder& Append(const ::std::string& key, const T&, const FirmwareVersion& target_version = FirmwareVersion{});

  ::std::string ToString(JsonFormat format = JsonFormat::COMPACT);
  nlohmann::json json_;

};


}
