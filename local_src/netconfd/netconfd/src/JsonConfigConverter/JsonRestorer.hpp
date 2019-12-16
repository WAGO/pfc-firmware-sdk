// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <functional>
#include <string>
#include <utility>
#include <nlohmann/json_fwd.hpp>

#include "Status.hpp"

namespace netconfd {

class JsonRestorer {
 public:
  explicit JsonRestorer(::std::string&& backup): backup_ { ::std::move(backup) } {};
  template<class T>
  Status Restore(const ::std::string& key, T&);
 private:
  Status Restore(const ::std::string& key, const ::std::function<Status(const ::nlohmann::json&)>& convert);
  const ::std::string backup_;
};

}/* namespace netconfd */
