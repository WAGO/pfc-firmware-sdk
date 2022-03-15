// SPDX-License-Identifier: GPL-2.0-or-later

#include "HostnameManager.hpp"

#include "LeaseFile.hpp"
#include "HostsFile.hpp"
#include "DefaultHostname.hpp"
#include "Logger.hpp"

namespace netconf {

namespace {

LeaseFile GetLeaseFile(const ::std::string &interface) {
  LeaseFile lease;
  lease.Parse(LeaseFile::GetLeaseFilePath(interface));
  return lease;
}

::std::vector<::std::string> GetIPsOfIPConfigs(const IPConfigs &ip_configs) {
  ::std::vector<::std::string> ips;
  for (auto &ip_config : ip_configs) {
    if (ip_config.address_ != ZeroIP) {
      ips.push_back(ip_config.address_);
    }
  }
  return ips;
}

void UpdateEtcHosts(const IPConfigs &ip_configs, const ::std::string &hostname, ::std::string& domain) {

  auto ips = GetIPsOfIPConfigs(ip_configs);
  UpdateHostsFile(ips, hostname, domain);
}

}

HostnameManager::HostnameManager() {
  default_hostname_ = GetDefaultHostname();

  auto host_conf = HostConfFile::ParseFile();
  hostname_ = host_conf.GetHostname().empty() ? default_hostname_ : host_conf.GetHostname();
  domain_ = host_conf.GetDomain();

  SetKernelHostname(hostname_);
  UpdateEtcHosts(GetCurrentIPConfigs(), hostname_, domain_);
}

::std::string HostnameManager::GetHostname() {
  return hostname_;
}

void HostnameManager::OnReloadHostConf() {

  auto old_hostname = hostname_;

  auto host_conf = HostConfFile::ParseFile();
  hostname_ = host_conf.GetHostname().empty() ? default_hostname_ : host_conf.GetHostname();
  domain_ = host_conf.GetDomain();

  LogInfo("use /etc/host.conf hostname: " + hostname_ + " domain: " + domain_);
  SetKernelHostname(hostname_);
  UpdateEtcHosts(GetCurrentIPConfigs(), hostname_, domain_);

  if (ip_manager_ != nullptr && hostname_ != old_hostname) {
    ip_manager_->OnHostnameChanged();
  }
}

void HostnameManager::OnLeaseFileChange(const std::string &interface) {

  auto lease_file = GetLeaseFile(interface);
  if (lease_file.Exists()) {
    prioritized_dhcp_host_domainname_.Add(interface, lease_file.GetDHCPHostname(), lease_file.GetDHCPDomain());
    prioritized_dhcp_host_domainname_.GetPrioritizedValues(hostname_, domain_);

    LogInfo("use lease file hostname: " + hostname_ + " domain: " + domain_ + " of interface: " + interface);
    SetKernelHostname(hostname_);
    UpdateEtcHosts(GetCurrentIPConfigs(), hostname_, domain_);
  } else {
    OnLeaseFileRemove(interface);
  }

}

void HostnameManager::OnInterfaceIPChange() {
  UpdateEtcHosts(GetCurrentIPConfigs(), hostname_, domain_);
}

void HostnameManager::OnLeaseFileRemove(const ::std::string &interface) {
  /*
   * It is checked whether a lease file exists for this interface and whether this has also set the current host name.
   * If so, the host name of the host.conf file (manual user setting) is set again when lease is invalid.
   */
  if (prioritized_dhcp_host_domainname_.GetHostname(interface) == hostname_) {
    OnReloadHostConf();
  }
  prioritized_dhcp_host_domainname_.Remove(interface);
}

void HostnameManager::RegisterIPManager(IIPManager &ip_manager) {
  ip_manager_ = &ip_manager;
}

IPConfigs HostnameManager::GetCurrentIPConfigs() {
  return (ip_manager_ != nullptr) ? ip_manager_->GetCurrentIPConfigs() : IPConfigs();
}

} /* namespace netconf */
