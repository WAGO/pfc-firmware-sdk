// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <type_traits>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace netconfd {

class JsonKeyList {
 public:

  JsonKeyList(std::initializer_list<std::string> args) : keys{std::move(args)}{
  }

  bool containsKey(const std::string& value) const {
    return std::find(keys.begin(), keys.end(), value) != keys.end();
  }

  bool matchesJsonObject(const nlohmann::json& tree) const {
    for(const auto& v : tree.items())
    {
      if(!containsKey(v.key()))
        return false;
    }
    return true;
  }

 private:
  const std::vector<std::string> keys;
};

} /* namespace netconfd */
