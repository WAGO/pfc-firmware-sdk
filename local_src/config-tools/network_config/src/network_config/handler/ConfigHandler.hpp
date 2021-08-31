// SPDX-License-Identifier: GPL-2.0-or-later

#include <string>
#include <iostream>
#include <exception>

#include "Utils.hpp"

namespace po = boost::program_options;

namespace network_config {

class ConfigHandler : public IHandler {
 public:
  virtual ConfigHandler(const po::variables_map &vm)
      : vm_ { vm } {
    if (vm_.count("get"))
      execute_ = [this]() {
        this->GetConfig();
      };
    else if (vm_.count("set"))
      execute_ = [this]() {
        this->SetConfig();
      };
  }

  virtual void Execute() {
    execute_();
  }

 protected:
  virtual void GetConfig() = 0;
  virtual void SetConfig() = 0;
};

} /* namespace network_config */
