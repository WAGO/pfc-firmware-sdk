// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

#include "Status.hpp"
#include "DynamicIPEventAction.hpp"

namespace netconf {

class IDynamicIPEventJsonConvert {
 public:
  virtual ~IDynamicIPEventJsonConvert() = default;

  virtual ::std::string ToJsonString(const::std::string& itf_name, DynamicIPEventAction action, JsonFormat format = JsonFormat::COMPACT) const noexcept = 0;
  virtual Status FromJsonString(const ::std::string &str, ::std::string& itf_name, DynamicIPEventAction& action) const noexcept = 0;

};

}  // namespace netconf

