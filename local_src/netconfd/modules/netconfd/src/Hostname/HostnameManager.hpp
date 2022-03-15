#pragma once

#include <string>
#include <map>
#include <tuple>

#include "HostConfFile.hpp"
#include "IHostnameManager.hpp"
#include "IHostnameWillChange.hpp"
#include "HostnameController.hpp"
#include "PrioritizedHostAndDomainname.hpp"

namespace netconf {

class HostnameManager : public IHostnameManager, public IHostnameWillChange {
 public:
  HostnameManager();
  virtual ~HostnameManager() = default;

  HostnameManager(const HostnameManager &other) = delete;
  HostnameManager(HostnameManager &&other)      = delete;
  HostnameManager &operator=(const HostnameManager &other) = delete;
  HostnameManager &operator=(HostnameManager &&other) = delete;

  ::std::string GetHostname() override;

  void OnReloadHostConf() override;
  void OnLeaseFileChange(const ::std::string &interface) override;
  void OnInterfaceIPChange() override;
  void OnLeaseFileRemove(const ::std::string &interface) override;

  void RegisterIPManager(IIPManager& ip_manager) override;

 private:
  IIPManager* ip_manager_ = nullptr;

  ::std::string default_hostname_;
  ::std::string hostname_;
  ::std::string domain_;

  PrioritizedHostAndDomainname prioritized_dhcp_host_domainname_;

  IPConfigs GetCurrentIPConfigs();
  void SetPrioritizedDHCPHostAndDomainname();

};

} /* namespace netconf */
