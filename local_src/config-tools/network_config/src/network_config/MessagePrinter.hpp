// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ostream>
#include <fstream>
#include <memory>
#include <utility>
#include <iostream>
#include <Status.hpp>
#include "Output.hpp"

namespace network_config {

class MessagePrinter {
 public:
  MessagePrinter();
  ~MessagePrinter() = default;

  void SetDestination(::std::unique_ptr<Output> output);
  void Print(const ::netconf::Status &status);
  void Print(const ::std::string &status);

 private:
  ::std::unique_ptr<Output> output_;

};

} /* namespace network_config */

