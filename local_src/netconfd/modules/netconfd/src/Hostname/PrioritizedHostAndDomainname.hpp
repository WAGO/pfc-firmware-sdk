#pragma once

#include <string>
#include <map>
#include <tuple>

namespace netconf {

class PrioritizedHostAndDomainname {
 public:
  PrioritizedHostAndDomainname() = default;
  virtual ~PrioritizedHostAndDomainname() = default;

  PrioritizedHostAndDomainname(const PrioritizedHostAndDomainname &other) = delete;
  PrioritizedHostAndDomainname(PrioritizedHostAndDomainname &&other) = delete;
  PrioritizedHostAndDomainname& operator=(const PrioritizedHostAndDomainname &other) = delete;
  PrioritizedHostAndDomainname& operator=(PrioritizedHostAndDomainname &&other) = delete;

  void Add(const ::std::string &interface, const ::std::string &hostname, const ::std::string &domain);
  void Remove(const ::std::string &interface);
  void GetPrioritizedValues(::std::string &hostname, ::std::string &domain);
  ::std::string GetHostname(const::std::string &interface);

 private:
  //Dynamic Host/Domain map map<Bridge, pair<Hostname, Domain>>
  ::std::map<::std::string, ::std::pair<::std::string, ::std::string>> dynamic_host_domain_map_;
};

}
