// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDynamicIPClient.hpp"
#include <string>
#include "BaseTypes.hpp"
#include "DynamicIPType.hpp"
#include "Status.hpp"

namespace netconf {

class ILeaseFile {
 public:
  ILeaseFile() = default;
  virtual ~ILeaseFile() = default;

  ILeaseFile(const ILeaseFile&) = default;
  ILeaseFile& operator=(const ILeaseFile&) = delete;
  ILeaseFile(const ILeaseFile&&) = delete;
  ILeaseFile& operator=(const ILeaseFile&&) = delete;

  virtual void Parse(const ::std::string &lease_file_path) = 0;
  virtual Address GetAddress() = 0;
  virtual Netmask GetNetmask() = 0;
  virtual ::std::string GetDHCPHostname() = 0;
  virtual ::std::string GetDHCPDomain() = 0;

};

} /* namespace netconf */
