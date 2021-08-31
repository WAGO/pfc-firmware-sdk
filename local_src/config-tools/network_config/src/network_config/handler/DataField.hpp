// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <functional>
#include <string>
#include <utility>

namespace network_config {

template<typename DataType>
class DataField {
 public:
  using ValueGetter = ::std::function<::std::string(const DataType&)>;
  using ValueSetter = ::std::function<void(DataType&,const ::std::string&)>;

  DataField(ValueGetter getter, ValueSetter setter = [](auto&, const ::std::string&) {
  }) noexcept
      : getter_ { std::move(getter) },
        setter_ { std::move(setter) } {
  }
  virtual ~DataField() = default;

  ::std::string Get(const DataType &data) {
    return getter_(data);
  }

  ::std::string Set(DataType &data, ::std::string value) {
    return setter_(data, value);
  }

 private:
  ValueGetter getter_;
  ValueSetter setter_;
};

} /* namespace network_config */

