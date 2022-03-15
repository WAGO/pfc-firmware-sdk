// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <string>
#include <chrono>

namespace netconf {

class InterprocessCondition {

 public:
  explicit InterprocessCondition(const ::std::string& name);
  ~InterprocessCondition() = default;

  bool Wait(std::chrono::milliseconds timeout);
  void Notify();

  void Remove();

 private:
  bool removed_ = false;
  ::std::string mutex_name_;
  ::std::string condition_name_;
  ::boost::interprocess::named_mutex mutex_;
  ::boost::interprocess::named_condition condition_;

};


} /* namespace netconf */

