// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDynamicIPClient.hpp"
#include "LeaseFile.hpp"

#include <string>
#include <glib.h>

namespace netconf {

class BOOTPClient : public IDynamicIPClient {
 public:
  BOOTPClient(::std::string itf_name);
  virtual ~BOOTPClient();

  BOOTPClient(const BOOTPClient &other) = delete;
  BOOTPClient(BOOTPClient &&other) = delete;
  BOOTPClient& operator=(const BOOTPClient &other) = delete;
  BOOTPClient& operator=(BOOTPClient &&other) = delete;

  void Release() override;
  void Renew() override;

  DynamicIPType GetType() override;

  void Restart(::std::string hostname) override;

  void UpdateContentFromLease() override;
  Address GetAddressFromLease() override;
  Netmask GetNetmaskFromLease() override;
  ::std::string GetHostnameFromLease() override;
  ::std::string GetDomainFromLease() override;

 private:
  LeaseFile lease_file_;

  ::std::string itf_name_;
  ::std::string pid_file_path_;

  GPid pid_;
  const ::std::string BOOTP_SCRIPT_PATH = "/sbin/bootpc-startup";

  void Start();
  void Stop();
};

} /* namespace netconf */

