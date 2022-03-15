// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace netconf {

class IHostnameWillChange {
 public:
  IHostnameWillChange() = default;
  virtual ~IHostnameWillChange() = default;

  IHostnameWillChange(const IHostnameWillChange&) = delete;
  IHostnameWillChange& operator=(const IHostnameWillChange&) = delete;
  IHostnameWillChange(IHostnameWillChange&&) = delete;
  IHostnameWillChange& operator=(IHostnameWillChange&&) = delete;

  virtual void OnReloadHostConf() = 0;

};



}  // namespace netconf
