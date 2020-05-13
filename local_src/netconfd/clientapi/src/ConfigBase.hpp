// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace netconf {

template <class T>
class ConfigBase {
 public:
  ConfigBase() = default;

  ::std::vector<T> configs_;

  void AddConfig(T& config) {
    auto matches_interface_name = [&](T& c) { return c.interface_ == config.interface_; };
    auto it                     = ::std::find_if(configs_.begin(), configs_.end(), matches_interface_name);
    if (it == configs_.end()) {
      configs_.push_back(config);
    } else {
      *it = config;
    }
  }

  boost::optional<T> GetConfig(const ::std::string& interface_name) {
    auto matches_interface_name = [&](const T& c) { return c.interface_ == interface_name; };
    auto it                     = ::std::find_if(configs_.cbegin(), configs_.cend(), matches_interface_name);
    if (it != configs_.cend()) {
      return *it;
    }
    return boost::optional<T>();
  }

  void RemoveConfig(const ::std::string& interface_name) {
    auto matches_interface_name = [&](T& c) { return c.interface_ == interface_name; };
    auto to_remove              = ::std::remove_if(configs_.begin(), configs_.end(), matches_interface_name);
    configs_.erase(to_remove);
  }
};

}  // namespace netconf
