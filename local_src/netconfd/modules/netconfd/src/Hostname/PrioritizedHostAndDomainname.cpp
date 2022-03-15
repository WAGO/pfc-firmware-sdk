/*
 * PrioritizedDHCPHostAndDomainname.cpp
 *
 *  Created on: 12.01.2022
 *      Author: u016903
 */

#include "PrioritizedHostAndDomainname.hpp"

#include "Logger.hpp"

namespace netconf {

namespace {

}

void PrioritizedHostAndDomainname::Add(const ::std::string &interface, const ::std::string &hostname,
                                       const ::std::string &domain) {
  dynamic_host_domain_map_.insert_or_assign(interface, ::std::pair { hostname, domain });

}

void PrioritizedHostAndDomainname::Remove(const ::std::string &interface) {
  if (dynamic_host_domain_map_.find(interface) != dynamic_host_domain_map_.end()) {
    dynamic_host_domain_map_.erase(interface);
  }
}

void PrioritizedHostAndDomainname::GetPrioritizedValues(::std::string &hostname, ::std::string &domain) {
  /*
   * Interfaces (bridges, wwan) are automatically sorted alphabetically in the dynamic_host_domain_map.
   * The interface index thus decides on the position in the map. (example br0, br1, br2, ..., wwan).
   * The lowest index has the highest priority and its host or domain name is set in the system.
   */

  for (auto& [interface, data] : dynamic_host_domain_map_) {
    auto entry_hostname = data.first;
    if (not entry_hostname.empty()) {
      hostname = entry_hostname;
      break;
    }
  }

  for (auto& [interface, data] : dynamic_host_domain_map_) {
    auto entry_domain = data.second;
    if (not entry_domain.empty()) {
      domain = entry_domain;
      break;
    }
  }
}

::std::string PrioritizedHostAndDomainname::GetHostname(const ::std::string &interface) {
  if (dynamic_host_domain_map_.find(interface) != dynamic_host_domain_map_.end()) {
    return dynamic_host_domain_map_[interface].first;
  }
  return "";
}

} /* namespace netconf */
