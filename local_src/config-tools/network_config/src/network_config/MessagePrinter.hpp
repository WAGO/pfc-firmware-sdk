// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ostream>
#include <fstream>
#include <memory>
#include <utility>
#include <iostream>
#include "Error.hpp"
#include "Output.hpp"

namespace network_config {

class MessagePrinter {
 public:
  MessagePrinter();
  ~MessagePrinter() = default;

  void SetDestination(::std::unique_ptr<Output> output);
  void Print(const ::netconf::Error &error);
  void Print(const ::std::string &error);

 private:
  ::std::unique_ptr<Output> output_;

};

} /* namespace network_config */

