// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <map>
#include "BaseTypes.hpp"
#include "DynamicIPType.hpp"

namespace netconf {

class IDynamicIPClient;

using IDynamicIPClientPtr = ::std::shared_ptr<IDynamicIPClient>;
using IDynamicIPClients = ::std::map<::std::string, IDynamicIPClientPtr>;

class IDynamicIPClient {
 public:
  IDynamicIPClient() = default;
  virtual ~IDynamicIPClient() = default;

  IDynamicIPClient(const IDynamicIPClient&) = delete;
  IDynamicIPClient& operator=(const IDynamicIPClient&) = delete;
  IDynamicIPClient(IDynamicIPClient&&) = delete;
  IDynamicIPClient& operator=(IDynamicIPClient&&) = delete;

  virtual void Release() = 0;
  virtual void Renew() = 0;

  virtual void Restart(::std::string hostname) = 0;

  virtual DynamicIPType GetType() = 0;

  virtual void UpdateContentFromLease() = 0;
  virtual Address GetAddressFromLease() = 0;
  virtual Netmask GetNetmaskFromLease() = 0;
  virtual ::std::string GetHostnameFromLease() = 0;
  virtual ::std::string GetDomainFromLease() = 0;
};

} /* namespace netconf */
