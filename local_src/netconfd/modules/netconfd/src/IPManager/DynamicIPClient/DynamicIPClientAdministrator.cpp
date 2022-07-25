// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPClientAdministrator.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "Logger.hpp"

namespace netconf {

namespace {

void DeleteAllLeaseFiles() {
  try {
    ::std::vector<::std::string> files;
    for (const auto &file : ::std::filesystem::directory_iterator("/tmp")) {
      files.emplace_back(file.path());
    }

    for (const auto &file : files) {
      if (file.rfind("/tmp/dhcp-bootp-data-", 0) == 0) {
        ::std::remove(file.c_str());
      }
    }
  } catch (const std::exception &e) {
    LogError("Error during initialization. An error occurred while deleting the existing lease files: " +
             ::std::string{e.what()});
  }
}

}  // namespace

DynamicIPClientAdministrator::DynamicIPClientAdministrator(::std::string vendorclass)
    : vendorclass_{::std::move(vendorclass)} {
  DeleteAllLeaseFiles();
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
