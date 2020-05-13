// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <memory>

namespace netconfd {

class IPLink;
class IPConfig;

class IIPLinks {
 public:
  IIPLinks() = default;
  virtual ~IIPLinks() = default;

  IIPLinks(const IIPLinks&) = delete;
  IIPLinks& operator=(const IIPLinks&) = delete;
  IIPLinks(const IIPLinks&&) = delete;
  IIPLinks& operator=(const IIPLinks&&) = delete;

  virtual ::std::shared_ptr<IPLink> CreateOrGet(const IPConfig& ip_config) = 0;
  virtual ::std::shared_ptr<IPLink> Get(const ::std::string &interface) = 0;
};

}
