// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <map>

namespace netconfd {

template <typename Key, typename Value>
class MappedChangeDetector {
 public:
  bool Change(const Key& k, Value newVal){
    if(change_map_.find(k) == change_map_.end())
    {
      change_map_.emplace(std::make_pair(k, newVal));
      return true;
    }
    else
    {
      auto oldval = change_map_.at(k);
      change_map_.at(k) = newVal;
      return oldval != newVal;
    }

  }

  Value GetValue(const Key& k) const {
    return change_map_.at(k);
  }

  void Remove(const Key& k){
    change_map_.erase(k);
  }

  void Clear(){
    change_map_.clear();
  }

 private:

  ::std::map<Key, Value> change_map_;
};

} /* namespace netconfd */
