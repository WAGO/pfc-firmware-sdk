// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <functional>
#include "FileDescriptor.hpp"
#include "IHostname.hpp"

namespace netconf {

class HostnameWatcher final : public IHostname{
 public:
  using UpdateFn = std::function<void(std::string)>;
  HostnameWatcher();
  HostnameWatcher(UpdateFn update_fn);
  ~HostnameWatcher();

  void UpdateHostname();

  std::string GetHostname() const override{
    return hostname_;
  }
  void SetHostname(const std::string& newname) override;

  void SetUpdateFunction(UpdateFn update_fn);

 private:
  FileDescriptor epoll_fd_;
  FileDescriptor hostname_fd_;
  uint gsource_ = 0;
  std::string hostname_;
  UpdateFn update_fn_;

  std::string ReadHostname();
};

} /* namespace netconf */

