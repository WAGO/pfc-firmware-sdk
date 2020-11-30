// SPDX-License-Identifier: GPL-2.0-or-later

#include "MessagePrinter.hpp"

namespace network_config {

MessagePrinter::MessagePrinter()
{
  output_ = ::std::make_unique<StderrOutput>();
}

void MessagePrinter::SetDestination(::std::unique_ptr<Output> output)
{
  output_ = std::move(output);
}

void MessagePrinter::Print(const ::netconf::Error &error){
  Print(error.ToString());
}

void MessagePrinter::Print(const ::std::string& text){
  if(output_){
    output_->GetStream() << text;
    output_->GetStream().flush();
  }
}

} /* namespace network_config */
