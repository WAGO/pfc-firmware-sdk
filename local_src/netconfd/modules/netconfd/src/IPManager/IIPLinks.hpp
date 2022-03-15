// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>
#include <memory>

namespace netconf {

class IPLink;

class IIPLinks {
 public:
  IIPLinks() = default;
  virtual ~IIPLinks() = default;

  IIPLinks(const IIPLinks&) = delete;
  IIPLinks& operator=(const IIPLinks&) = delete;
  IIPLinks(IIPLinks&&) = delete;
  IIPLinks& operator=(IIPLinks&&) = delete;

  virtual ::std::shared_ptr<IPLink> CreateOrGet(const ::std::string& interface) = 0;
};

}
