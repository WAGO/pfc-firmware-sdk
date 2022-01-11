
#pragma once

#include <string>

namespace netconf {

class IHostname {
 public:
  IHostname() = default;
  virtual ~IHostname() = default;

  IHostname(const IHostname&) = delete;
  IHostname& operator=(const IHostname&) = delete;
  IHostname(IHostname&&) = delete;
  IHostname& operator=(IHostname&&) = delete;


  virtual std::string GetHostname() const = 0;
  virtual void SetHostname(const std::string& newname) = 0;

};



}  // namespace netconf
