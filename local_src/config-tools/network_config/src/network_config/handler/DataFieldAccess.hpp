// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <map>

#include "InterfaceInformationApi.hpp"
#include "DataField.hpp"

namespace network_config {

template<typename T>
class DataFieldAccess {
 public:

  template<typename U>
  using FieldMap = ::std::map<::std::string, DataField<U>>;

  DataFieldAccess();
  virtual ~DataFieldAccess() = default;

  ::std::string Get(const T &info, ::std::string field_name) {
    if (field_map_.count(field_name) > 0) {
      return field_map_.at(field_name).Get(info);
    }
    return ::std::string { };
  }

 private:
  FieldMap<T> field_map_;
};

} /* namespace network_config */

