// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <boost/optional.hpp>
#include <string>
#include <vector>
#include <functional>
#include "Error.hpp"

namespace netconf {
namespace api {
namespace detail {

template<class T>
class ConfigBase {
 protected:
  explicit ConfigBase(const ::std::vector<T> &config)
      : configs_ { config } {
  }
  ConfigBase() = default;
  virtual ~ConfigBase() = default;

  virtual ::std::string GetCompareValue(const T&) const noexcept = 0;
  ::std::vector<T> configs_;

 public:

  ::std::vector<T> GetConfig() const noexcept {
    return configs_;
  }

  void AddConfig(T &config) {
    auto matches_interface_name = [&](T &c) {
      return GetCompareValue(c) == GetCompareValue(config);
    };
    auto it = ::std::find_if(configs_.begin(), configs_.end(), matches_interface_name);
    if (it == configs_.end()) {
      configs_.push_back(config);
    } else {
      *it = config;
    }
  }

  boost::optional<T> GetConfig(const ::std::string &interface_name) const {
    auto matches_interface_name = [&](const T &c) {
      return GetCompareValue(c) == interface_name;
    };
    auto it = ::std::find_if(configs_.cbegin(), configs_.cend(), matches_interface_name);
    if (it != configs_.cend()) {
      return *it;
    }
    return boost::optional<T>();
  }

  void RemoveConfig(const ::std::string &interface_name) {
    auto matches_interface_name = [&](T &c) {
      return GetCompareValue(c) == interface_name;
    };
    auto to_remove = ::std::remove_if(configs_.begin(), configs_.end(), matches_interface_name);
    configs_.erase(to_remove);
  }

};

}  // namespace detail
}  // namespace api
}  // namespace netconf
