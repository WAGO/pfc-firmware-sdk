#pragma once

#include "IHandler.hpp"

#include <functional>
#include <string>
#include <boost/program_options.hpp>

namespace network_config {

class ReloadHostConfHandler : public IHandler {
 public:
  ReloadHostConfHandler() = default;
  virtual ~ReloadHostConfHandler() override = default;

  ReloadHostConfHandler(const ReloadHostConfHandler &other) = delete;
  ReloadHostConfHandler(ReloadHostConfHandler &&other) = delete;
  ReloadHostConfHandler& operator=(const ReloadHostConfHandler &other) = delete;
  ReloadHostConfHandler& operator=(ReloadHostConfHandler &&other) = delete;

  void Execute() override;
};

}
