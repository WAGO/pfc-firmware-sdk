// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace netconfd {

class JsonBuilder {

 public:
  template<class T> JsonBuilder& Append(const ::std::string& key, const T&);

  ::std::string ToString();
  nlohmann::json json_;

};


}
