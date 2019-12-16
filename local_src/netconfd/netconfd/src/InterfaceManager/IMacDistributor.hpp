// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../../../extern/Status.hpp"
#include "Types.hpp"

namespace netconfd {

class IMacDistributor {
 public:
  IMacDistributor() = default;
  virtual ~IMacDistributor() = default;
  IMacDistributor(const IMacDistributor&) = delete;
  IMacDistributor& operator=(const IMacDistributor&) = delete;
  IMacDistributor(IMacDistributor&&) = delete;
  IMacDistributor& operator=(IMacDistributor&&) = delete;

  virtual Status SetMac(Bridge const& interface) = 0;


};

} /* namespace netconfd */
