// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterprocessCondition.h"
#include <boost/date_time/posix_time/conversion.hpp>

namespace netconf {

using scoped_lock = ::boost::interprocess::scoped_lock<::boost::interprocess::named_mutex>;

InterprocessCondition::InterprocessCondition(const ::std::string& name)
    : mutex_name_ { name + "_mutex" },
      condition_name_ { name + "_condition" },
      mutex_ { boost::interprocess::open_or_create, mutex_name_.c_str() },
      condition_ { boost::interprocess::open_or_create, condition_name_.c_str() } {
}

bool InterprocessCondition::Wait(std::chrono::milliseconds timeout) {
  if (removed_) {
    throw std::runtime_error("Interprocess condition already removed");
  }

  using ::std::chrono::system_clock;
  scoped_lock l { mutex_ };

  auto timeout_abs = system_clock::now() + timeout;
  return condition_.timed_wait(l, boost::posix_time::from_time_t(system_clock::to_time_t(timeout_abs)));
}

void InterprocessCondition::Notify() {
  if (removed_){
    throw std::runtime_error("Interprocess condition already removed");
  }

  scoped_lock l { mutex_ };
  condition_.notify_all();
}

void InterprocessCondition::Remove() {
  if (removed_){
    throw std::runtime_error("Interprocess condition already removed");
  }

  removed_ = true;
  boost::interprocess::named_mutex::remove(mutex_name_.c_str());
  boost::interprocess::named_condition::remove(condition_name_.c_str());
}

} /* namespace netconf */
