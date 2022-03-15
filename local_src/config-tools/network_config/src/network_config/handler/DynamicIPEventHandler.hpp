#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

namespace network_config {

class DynamicIPEventHandler : public IHandler {
 public:
  DynamicIPEventHandler(const ::boost::program_options::variables_map &vm);
  virtual ~DynamicIPEventHandler() override = default;

  DynamicIPEventHandler(const DynamicIPEventHandler &other) = delete;
  DynamicIPEventHandler(DynamicIPEventHandler &&other) = delete;
  DynamicIPEventHandler& operator=(const DynamicIPEventHandler &other) = delete;
  DynamicIPEventHandler& operator=(DynamicIPEventHandler &&other) = delete;

  void Execute() override;
 private:
  void SetConfig();

  ::std::function<void()> execute_;
  const ::boost::program_options::variables_map &vm_;
};

}
