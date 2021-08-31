// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include <functional>
#include <string>
#include <utility>
#include <nlohmann/json_fwd.hpp>

#include "Status.hpp"

namespace netconf {

class JsonRestorer {
 public:
  explicit JsonRestorer(::std::string &&backup)
      : backup_ { ::std::move(backup) } {
  };

  template<class T>
  Status Restore(const ::std::string &key, T&);
 private:
  const ::std::string backup_;
};

}/* namespace netconf */
