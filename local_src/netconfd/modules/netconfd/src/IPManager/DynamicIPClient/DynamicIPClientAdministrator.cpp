// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPClientAdministrator.hpp"

namespace netconf {

DynamicIPClientAdministrator::DynamicIPClientAdministrator(const ::std::string& vendorclass)
    : vendorclass_ { vendorclass } {

}

IDynamicIPClientPtr DynamicIPClientAdministrator::AddClient(DynamicIPType type, const ::std::string &itf_name,
                                                            const ::std::string &hostname) {
  auto client = factory_.Create(type, itf_name, hostname, vendorclass_);
  clients_.emplace(itf_name, client);
  return client;
}

void DynamicIPClientAdministrator::DeleteClient(const ::std::string &itf_name) {
  clients_.erase(itf_name);
}

IDynamicIPClientPtr DynamicIPClientAdministrator::GetClient(const ::std::string &itf_name) const {
  auto it = clients_.find(itf_name);
  if (it != clients_.end()) {
    return (*it).second;
  }
  return nullptr;
}

void DynamicIPClientAdministrator::RestartAllClients(const ::std::string &hostname) {
  for (auto &client : clients_) {
    client.second->Restart(hostname);
  }
}

} /* namespace netconf */
