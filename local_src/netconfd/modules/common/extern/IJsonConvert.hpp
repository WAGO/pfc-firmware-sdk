// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

#include "Status.hpp"

namespace netconf {

enum class JsonFormat {
  COMPACT,
  PRETTY
};

template<class DataType>
class IJsonConvert {
 public:

  virtual ~IJsonConvert() = default;

  virtual ::std::string ToJsonString(const DataType &obj, JsonFormat format = JsonFormat::COMPACT) const noexcept = 0;
  virtual Status FromJsonString(const ::std::string &str, DataType &out_obj) const noexcept = 0;

};

}  // namespace netconf

