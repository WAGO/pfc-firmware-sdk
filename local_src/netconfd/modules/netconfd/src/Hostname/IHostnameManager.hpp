// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <functional>

#include "IIPManager.hpp"

namespace netconf {


class IHostnameManager {
 public:
  IHostnameManager() = default;
  virtual ~IHostnameManager() = default;

  IHostnameManager(const IHostnameManager&) = delete;
  IHostnameManager& operator=(const IHostnameManager&) = delete;
  IHostnameManager(IHostnameManager&&) = delete;
  IHostnameManager& operator=(IHostnameManager&&) = delete;

  virtual ::std::string GetHostname() = 0;
  virtual void OnLeaseFileRemove(const ::std::string &itf_name) = 0;
  virtual void OnLeaseFileChange(const ::std::string &itf_name) = 0;
  virtual void OnInterfaceIPChange() = 0;
  virtual void RegisterIPManager(IIPManager& ip_manager) = 0;

};

}  // namespace netconf
