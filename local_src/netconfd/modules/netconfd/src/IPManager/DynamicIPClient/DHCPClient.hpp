// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDynamicIPClient.hpp"
#include "LeaseFile.hpp"
#include <string>

#include <glib.h>

namespace netconf {

enum class DHCPClientStatus {
  RUNNING,
  STOPPED,
};

class DHCPClient : public IDynamicIPClient {
 public:
  DHCPClient(const ::std::string &itf_name, const ::std::string &hostname, const ::std::string &vendorclass);
  virtual ~DHCPClient();

  DHCPClient(const DHCPClient &other) = delete;
  DHCPClient(DHCPClient &&other) = delete;
  DHCPClient& operator=(const DHCPClient &other) = delete;
  DHCPClient& operator=(DHCPClient &&other) = delete;

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
  const ::std::string DHCP_CLIENT_PATH = "/sbin/udhcpc";

  LeaseFile lease_file_;

  GPid pid_;
  ::std::string hostname_;
  ::std::string itf_name_;
  ::std::string vendorclass_;
  ::std::string pid_file_path_;


  void Start(const ::std::string &hostname, const ::std::string &vendorclass);
  void Stop();
};

} /* namespace netconf */
